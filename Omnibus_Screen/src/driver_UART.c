/**
  ******************************************************************************
  * @file           : driver_UART.c
  * @brief          : 
  * @author         : Samuel Crepeault
  * 
  ******************************************************************************
  * @details
  * 
  * Function order allows not to use protoypes: compiler sees them before they
  * are used.
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "driver_UART.h"
#include "hardware/structs/uart.h"    /* uart_hw_t, uart_get_hw()  */
#include "hardware/structs/resets.h"  /* resets_hw                 */
#include "hardware/regs/uart.h"       /* UART_UARTxxx_BITS / _LSB  */
#include "hardware/regs/resets.h"     /* RESETS_RESET_UARTx_BITS   */
#include "hardware/clocks.h"          /* clock_get_hz(clk_peri)    */
#include "hardware/gpio.h"            /* gpio_set_function()       */
#include "pico/stdlib.h"              /* tight_loop_contents()     */

/* Defines -------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/

/* Functions -----------------------------------------------------------------*/

//==============================================================================
static void uart_reset_block(uart_inst_t *uart)
/* Hold the UART block in reset, release it, wait for it to come back.
 * Doing this guarantees a known-good register state regardless of what
 * a previous firmware image (or the bootrom) left configured.        */
//==============================================================================
{
    const uint32_t bit = (uart == uart0)
                       ? RESETS_RESET_UART0_BITS
                       : RESETS_RESET_UART1_BITS;

    hw_set_bits(&resets_hw->reset, bit);     /* assert reset          */
    hw_clear_bits(&resets_hw->reset, bit);   /* deassert              */
    while (!(resets_hw->reset_done & bit)) {
        tight_loop_contents();               /* spin until ready      */
    }
}

//==============================================================================
static void uart_set_baud_reg(uart_hw_t *hw, uint baud)
/* ------------------------------------------------------------------ */
/* PL011 baud divisor. This mirrors the SDK's own computation so the
 * result matches exactly; clk_peri is read at runtime (150 MHz default
 * on RP2350, but never hard-code it -- if you re-clock the peri domain
 * a hard-coded value silently corrupts every baud rate).             */
//==============================================================================
{
    const uint clk = clock_get_hz(clk_peri);

    /* 8x oversampled divisor, fixed-point .7 */
    uint32_t div  = (8u * clk) / baud;
    uint32_t ibrd = div >> 7;
    uint32_t fbrd;

    if (ibrd == 0u) {
        ibrd = 1u;
        fbrd = 0u;                 /* clamp: requested baud too high  */
    } else if (ibrd >= 65535u) {
        ibrd = 65535u;
        fbrd = 0u;                 /* clamp: requested baud too low   */
    } else {
        fbrd = ((div & 0x7fu) + 1u) / 2u;
    }

    hw->ibrd = ibrd;
    hw->fbrd = fbrd;
    /* NOTE: divisors are not latched until the next write to LCR_H,
     * which the caller does immediately after this returns.          */
}

//==============================================================================
void driver_UART_init(uart_inst_t *uart, uint tx_pin, uint rx_pin, uint baud)
//
//==============================================================================
{
    uart_hw_t *hw = uart_get_hw(uart);

    uart_reset_block(uart);

    /* Route the pins to the UART before enabling, so no garbage edge
     * is driven onto a half-configured peripheral.                   */
    gpio_set_function(tx_pin, GPIO_FUNC_UART);
    gpio_set_function(rx_pin, GPIO_FUNC_UART);

    /* UART must be disabled while we touch LCR_H / baud.             */
    hw->cr = 0u;

    uart_set_baud_reg(hw, baud);

    /* 8 data bits (WLEN = 0b11), FIFOs enabled, no parity, 1 stop.
     * Writing LCR_H here also latches IBRD/FBRD set just above.       */
    hw->lcr_h = (0x3u << UART_UARTLCR_H_WLEN_LSB)
              |  UART_UARTLCR_H_FEN_BITS;

    /* Enable DMA requests on both directions. DMAONERR masks the RX
     * DMA request line when a receive error is latched, so a framing
     * or break error can't quietly feed corrupt bytes into the DMA.  */
    hw->dmacr = UART_UARTDMACR_RXDMAE_BITS
              | UART_UARTDMACR_TXDMAE_BITS
              | UART_UARTDMACR_DMAONERR_BITS;

    /* Enable UART + TX + RX last.                                    */
    hw->cr = UART_UARTCR_UARTEN_BITS
           | UART_UARTCR_TXE_BITS
           | UART_UARTCR_RXE_BITS;
}