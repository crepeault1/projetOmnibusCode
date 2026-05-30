#include "DMA.h"
#include "frame.h"

#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/structs/uart.h"
#include "hardware/sync.h"   /* __dmb() */
#include <string.h>

/* ------------------------------------------------------------------ */
/* RX ping-pong buffers. The DMA writes one while the CPU (in the ISR)
 * hands off the other. aligned(4) is a good habit even though byte
 * transfers on RP2350 don't strictly require it -- it keeps the same
 * code portable to MCUs whose DMA does require alignment.            */
static volatile uint8_t s_rx_buf[2][UART_FRAME_SIZE] __attribute__((aligned(4)));
static volatile uint8_t s_active_rx = 0;   /* buffer DMA is filling now */

/* Single TX staging buffer. Frames are always 24 B, but we keep a
 * length-aware copy so the layer stays usable for shorter transfers. */
static uint8_t s_tx_buf[UART_FRAME_SIZE] __attribute__((aligned(4)));

static int s_rx_chan = -1;
static int s_tx_chan = -1;

/* ------------------------------------------------------------------ */
/* Shared handler for DMA_IRQ_0. BOTH channels route their completion
 * here, so we must test and clear each channel's bit independently --
 * never assume only one fired.                                       */
static void dma_irq0_handler(void)
{
    /* ---- RX completed: one full 24-byte frame landed ------------- */
    if (dma_hw->ints0 & (1u << s_rx_chan)) {
        dma_hw->ints0 = (1u << s_rx_chan);          /* W1C the flag   */

        uint8_t filled = s_active_rx;
        s_active_rx ^= 1u;                          /* flip target    */

        /* Re-arm onto the OTHER buffer FIRST. Any bytes arriving in
         * the gap sit in the 32-deep RX FIFO; re-arming before the
         * handoff keeps that gap as short as possible. The transfer
         * count is restored implicitly because the channel's CHAIN/
         * count was set to UART_FRAME_SIZE at config time and we only
         * move the write address here -- but to be explicit and
         * robust we set both. */
        dma_channel_set_trans_count(s_rx_chan, UART_FRAME_SIZE, false);
        dma_channel_set_write_addr(s_rx_chan,
                                   (void *)s_rx_buf[s_active_rx],
                                   true /* trigger */);

        __dmb(); /* publish the buffer write before the service layer
                    (possibly on the other core) reads it */

        uart_rx_frame_ready_from_isr(s_rx_buf[filled]);
    }

    /* ---- TX completed -------------------------------------------- */
    if (dma_hw->ints0 & (1u << s_tx_chan)) {
        dma_hw->ints0 = (1u << s_tx_chan);          /* W1C the flag   */
        uart_tx_complete_from_isr();
    }
}

/* ------------------------------------------------------------------ */
void dma_uart_init(uart_inst_t *uart)
{
    uart_hw_t *hw = uart_get_hw(uart);

    s_rx_chan = dma_claim_unused_channel(true);
    s_tx_chan = dma_claim_unused_channel(true);

    /* ---- RX channel: UART data register -> ping-pong buffer ------ */
    dma_channel_config rc = dma_channel_get_default_config(s_rx_chan);
    channel_config_set_transfer_data_size(&rc, DMA_SIZE_8);                 //Check name
    channel_config_set_read_increment(&rc, false);  /* DR is fixed    */
    channel_config_set_write_increment(&rc, true);  /* walk buffer    */
    channel_config_set_dreq(&rc, uart_get_dreq(uart, false /*rx*/));

    dma_channel_configure(
        s_rx_chan, &rc,
        (void *)s_rx_buf[s_active_rx],  /* initial write addr        */
        &hw->dr,                        /* read addr (UART DR)       */
        UART_FRAME_SIZE,                /* transfer count            */
        true /* start now: it parks on DREQ until bytes arrive */);

    /* ---- TX channel: staging buffer -> UART data register -------- */
    dma_channel_config tc = dma_channel_get_default_config(s_tx_chan);
    channel_config_set_transfer_data_size(&tc, DMA_SIZE_8);
    channel_config_set_read_increment(&tc, true);   /* walk buffer    */
    channel_config_set_write_increment(&tc, false); /* DR is fixed    */
    channel_config_set_dreq(&tc, uart_get_dreq(uart, true /*tx*/));

    /* Configure but do NOT start; start happens in dma_uart_start_tx. */
    dma_channel_set_config(s_tx_chan, &tc, false);
    dma_channel_set_write_addr(s_tx_chan, &hw->dr, false);

    /* ---- IRQ wiring: both channels on DMA_IRQ_0 ------------------ */
    dma_channel_set_irq0_enabled(s_rx_chan, true);
    dma_channel_set_irq0_enabled(s_tx_chan, true);
    irq_set_exclusive_handler(DMA_IRQ_0, dma_irq0_handler);
    irq_set_enabled(DMA_IRQ_0, true);
}

/* ------------------------------------------------------------------ */
void dma_uart_start_tx(const uint8_t *src, size_t len)
{
    if (len == 0u || len > UART_FRAME_SIZE) {
        return; /* 0-count DMA is undefined on RP2350; reject it */
    }

    memcpy(s_tx_buf, src, len);
    __dmb(); /* ensure the copy is visible before the DMA reads it */

    dma_channel_set_read_addr(s_tx_chan, s_tx_buf, false);
    dma_channel_set_trans_count(s_tx_chan, len, true /* trigger */);
}

void dma_uart_abort_tx(void)
{
    dma_channel_abort(s_tx_chan);
    /* abort can leave the completion flag asserted; clear it so the
     * next legitimate completion isn't swallowed or double-counted. */
    dma_hw->ints0 = (1u << s_tx_chan);
}
