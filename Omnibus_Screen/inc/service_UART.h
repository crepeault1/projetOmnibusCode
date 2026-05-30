#ifndef SERVICE_UART_H
#define SERVICE_UART_H
/**
  ******************************************************************************
  * @file           : template.h
  * @brief          : Template for other .h files
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
#include "hardware/uart.h"
#include <stdbool.h>
#include <stdint.h>

/* Defines -------------------------------------------------------------------*/

/* Variables ---------------------------------------------------------------- */

/* Prototypes ----------------------------------------------------------------*/


/*
 * Application-facing UART service. Brings up the driver + DMA layers
 * and exposes blocking-with-timeout read/write backed by semaphores.
 *
 *   read:  rx_sem counts queued frames (released from the RX ISR);
 *          the actual bytes live in a multicore-safe queue.
 *   write: tx_mutex serializes writers; tx_done_sem is released by the
 *          TX-complete ISR.
 */
void service_UART_init(uart_inst_t *uart, uint tx_pin, uint rx_pin, uint baud);

/* Wait up to timeout_ms for a frame. On success copies UART_FRAME_SIZE
 * bytes into dst and returns true. dst must hold at least 24 bytes. */
bool uart_service_read(uint8_t *dst, uint32_t timeout_ms);

/* Send one frame (UART_FRAME_SIZE bytes). Blocks until the TX DMA
 * completes or timeout_ms elapses. Returns false on timeout. */
bool uart_service_write(const uint8_t *frame, uint32_t timeout_ms);

/* Count of frames dropped because the RX queue was full when the ISR
 * tried to enqueue them. Non-zero means the consumer can't keep up. */
uint32_t uart_service_rx_overruns(void);

#endif /* SERVICE_UART_H */
