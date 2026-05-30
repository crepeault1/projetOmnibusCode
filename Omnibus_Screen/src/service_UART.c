/**
  ******************************************************************************
  * @file           : service_UART.c
  * @brief          : 
  * @author         : Samuel Crepeault
  * 
  ******************************************************************************
  * @details
  * 
  * 
  *
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "service_UART.h"
#include "driver_UART.h"
#include "DMA.h"
#include "frame.h"

#include "pico/sync.h"        /* semaphore_t, mutex_t */
#include "pico/util/queue.h"  /* queue_t (multicore-safe) */
#include <string.h>

/* Defines -------------------------------------------------------------------*/
/* Depth of the RX backlog. The semaphore max and queue depth MUST match
 * so the two counts can never diverge. */
#define RX_QUEUE_DEPTH 8u

/* Variables -----------------------------------------------------------------*/

static semaphore_t s_rx_sem;       /* counts frames waiting in s_rx_q   */
static semaphore_t s_tx_done_sem;  /* 0/1: TX DMA finished              */
static mutex_t     s_tx_mutex;     /* one writer at a time              */
static queue_t     s_rx_q;         /* holds copies of received frames   */

static volatile uint32_t s_rx_overruns = 0;

/* Functions -----------------------------------------------------------------*/

void service_UART_init(uart_inst_t *uart, uint tx_pin, uint rx_pin, uint baud)
{
    /* Sync primitives must exist before any ISR can fire, so set them
     * up before bringing the hardware/DMA up. */
    sem_init(&s_rx_sem, 0, RX_QUEUE_DEPTH);
    sem_init(&s_tx_done_sem, 0, 1);
    mutex_init(&s_tx_mutex);
    queue_init(&s_rx_q, UART_FRAME_SIZE, RX_QUEUE_DEPTH);

    driver_UART_init(uart, tx_pin, rx_pin, baud);  /* enables UART + DMA reqs */
    dma_uart_init(uart);                       /* arms RX, starts parking */
}

/* ---- DMA event hooks (called from ISR context) ------------------- */

void uart_rx_frame_ready_from_isr(volatile uint8_t *frame)
{
    /* Copy the 24 bytes into the queue HERE, in the ISR. This transfers
     * ownership immediately so the ping-pong buffer is free to be re-
     * used the instant the next frame arrives -- the consumer can never
     * race the DMA over a shared buffer. The cost is one 24-byte copy
     * per frame in ISR context, which is negligible at these sizes.
     *
     * (For large/variable payloads where copying in-ISR is too costly,
     * switch to a pool of N physical buffers and enqueue POINTERS, with
     * the consumer returning buffers to the pool when done.)            */
    if (queue_try_add(&s_rx_q, (const void *)frame)) {
        sem_release(&s_rx_sem);          /* ISR-safe */
    } else {
        s_rx_overruns++;                 /* consumer fell behind */
    }
}

void uart_tx_complete_from_isr(void)
{
    sem_release(&s_tx_done_sem);
}

/* ---- public API -------------------------------------------------- */

bool uart_service_read(uint8_t *dst, uint32_t timeout_ms)
{
    if (!sem_acquire_timeout_ms(&s_rx_sem, timeout_ms)) {
        return false;                    /* nothing arrived in time */
    }

    /* sem and queue counts are kept in lockstep, so a successful
     * acquire guarantees a successful remove. */
    if (!queue_try_remove(&s_rx_q, dst)) {
        return false;                    /* should never happen */
    }

    /* Cheap structural sanity check. EOF lives somewhere in [1..23];
     * the caller scans for FRAME_EOF to find the true content end. */
    return (dst[0] == FRAME_SOF);
}

bool uart_service_write(const uint8_t *frame, uint32_t timeout_ms)
{
    mutex_enter_blocking(&s_tx_mutex);

    /* Clear any stale completion left by a previously timed-out write,
     * so this call only sees ITS OWN completion. */
    sem_reset(&s_tx_done_sem, 0);

    dma_uart_start_tx(frame, UART_FRAME_SIZE);

    bool ok = sem_acquire_timeout_ms(&s_tx_done_sem, timeout_ms);
    if (!ok) {
        /* Tear down the stuck transfer so it can't fire late and
         * release the semaphore for the next, unrelated writer. */
        dma_uart_abort_tx();
    }

    mutex_exit(&s_tx_mutex);
    return ok;
}

uint32_t uart_service_rx_overruns(void)
{
    return s_rx_overruns;
}
