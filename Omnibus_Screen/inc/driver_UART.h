#ifndef DRIVER_UART_H
#define DRIVER_UART_H
/**
  ******************************************************************************
  * @file           : driver_UART.h
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
#include "hardware/uart.h"   /* uart_inst_t, uart0/uart1 */
#include <stdint.h>

/* Defines -------------------------------------------------------------------*/

/* Variables ---------------------------------------------------------------- */

/* Prototypes ----------------------------------------------------------------*/



/*
 * Bring up a hardware UART at the register level:
 *   - resets the peripheral
 *   - sets baud (computed against clk_peri)
 *   - 8-N-1, FIFOs enabled
 *   - enables RX *and* TX DMA request lines (UARTDMACR)
 *   - enables the UART
 *
 * Pin muxing uses the SDK's gpio_set_function() purely for the IO bank
 * mux (doing the pads_bank0/io_bank0 registers by hand is verbose and a
 * common source of silent bring-up bugs). Everything UART-specific is
 * done through the raw uart_hw_t registers.
 */
void driver_UART_init(uart_inst_t *uart, uint tx_pin, uint rx_pin, uint baud);

#endif /* DRIVER_UART_H */
