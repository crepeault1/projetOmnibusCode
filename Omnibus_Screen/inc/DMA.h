#ifndef DMA_UART_H
#define DMA_UART_H

#include "hardware/uart.h"
#include <stddef.h>
#include <stdint.h>

/*
 * DMA layer: owns two channels (RX + TX) and the RX ping-pong buffers.
 * Call uart_hw_init() (driver layer) BEFORE this so the UART is already
 * enabled with its DMA request lines on.
 */
void dma_uart_init(uart_inst_t *uart);

/* Kick off a TX. Copies len bytes into an internal aligned TX buffer,
 * then starts the TX DMA channel. Completion is reported asynchronously
 * via uart_tx_complete_from_isr() (implemented by the service layer).
 * len must be <= UART_FRAME_SIZE. */
void dma_uart_start_tx(const uint8_t *src, size_t len);

/* Abort an in-flight TX (used by the service layer on write timeout). */
void dma_uart_abort_tx(void);

/* ---- event hooks ----------------------------------------------------
 * These are implemented by the SERVICE layer. The DMA layer raises them
 * from ISR context; it never includes the service layer's internals.
 * Loose coupling: swap these for a registered function pointer if you
 * prefer runtime binding over link-time binding.
 * ------------------------------------------------------------------- */
extern void uart_rx_frame_ready_from_isr(volatile uint8_t *frame);
extern void uart_tx_complete_from_isr(void);

#endif /* DMA_UART_H */
