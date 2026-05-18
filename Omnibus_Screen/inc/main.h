#ifndef MAIN_H
#define MAIN_H
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Main program header
 * @author         : Samuel Crepeault
 *
 ******************************************************************************
 * @details
 *                       Raspberry Pi Pico 2W
 *                        __________________
 *  UART0 TX - GP0 -  1  | O      USB     O |  40 - VBUS
 *  UART0 TX - GP1 -  2  |                  |  39 - VSYS
 *             GND -  3  |                  |  38 - GND
 *   I2C SDA - GP2 -  4  |                  |  37 - 3V3_EN
 *   I2C SCL - GP3 -  5  |  _               |  36 - 3V3_OUT
 *       SW1 - GP4 -  6  | |_|              |  35 - ADC_VREF
 *       SW2 - GP5 -  7  |                  |  34 - GP28 - ENC_BUT
 *             GND -  8  |                  |  33 - GND
 *       SW3 - GP6 -  9  |                  |  32 - GP27 - ENC_B
 *   BUF2_EN - GP7 - 10  |     ________     |  31 - GP26 - ENC_A
 *         A - GP8 - 11  |    |        |    |  30 - RUN
 *         B - GP9 - 12  |    | RP2350 |    |  29 - GP22 - B1
 *             GND - 13  |    |________|    |  28 - GND
 *        C - GP10 - 14  |                  |  27 - GP21 - G1
 *        D - GP11 - 15  |                  |  26 - GP20 - R1
 *        E - GP12 - 16  |                  |  25 - GP19 - B0
 *      CLK - GP13 - 17  |                  |  24 - GP18 - G0
 *             GND - 18  |                  |  23 - GND
 *      LAT - GP14 - 19  | O              O |  22 - GP17 - R0
 *       OE - GP15 - 20  |__________________|  21 - GP16 - BUF1_EN
 * 
 * @attention
 * 3V3_EN devrait etre au GND.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/uart.h"
/* Defines -------------------------------------------------------------------*/
//UART
#define UART_ID uart0 // stdout sur usb
#define BAUD_RATE 115200
#define UART_TX_PIN 0
#define UART_RX_PIN 1

//I2C
#define I2C_PORT i2c1 // 400khz
#define I2C_SDA 2
#define I2C_SCL 3

//Switches
#define SW1_PIN 4
#define SW2_PIN 5
#define SW3_PIN 6

//Buffer enables
#define BUF2_EN_PIN 7
#define BUF1_EN_PIN 16

//HUB75 lines
#define ROWSEL_BASE_PIN 8
#define ROWSEL_N_PINS 5  //5 car ABCDE, sinon 4
#define CLK_PIN 13
#define STROBE_PIN 14
#define OEN_PIN 15
#define DATA_BASE_PIN 17
#define DATA_N_PINS 6

//Encoder
#define ENC_A_PIN 26
#define ENC_B_PIN 27
#define ENC_BUT_PIN 28


//Service Scheduler Start------------------
#define SCHEDULER_PHASES 12

//Data IN
#define PHASE_SERVICE_SERIAL_IN 0 
#define PHASE_INTERFACE_BUTTON1 1
#define PHASE_INTERFACE_BUTTON2 2
#define PHASE_INTERFACE_BUTTON3 3
#define PHASE_INTERFACE_ROTARY_ENC_BUTTON 4
#define PHASE_INTERFACE_ROTARY_ENC 5

//Data MANAGEMENT
#define PHASE_PROCESS_BUTTONS 6
#define PHASE_PROCESS_ROTARY_ENC 7
#define PHASE_PROCESS_UI 8

//Data OUT
#define PHASE_PROCESS_FRAME_ASSEMBLY 9
#define PHASE_SERVICE_SERIAL_OUT 10
#define PHASE_DRIVER_HUB75E_OUT 11
//Service Scheduler End--------------------



#define INFORMATION_AVAILABLE  1
#define INFORMATION_HANDLED  0
#define REQUEST_ACTIVE  1
#define REQUEST_HANDLED 0
#define CLOCKWISE 1
#define COUNTERCLOCKWISE 0
#define UNKNOWN 2

#define OSC_FREQ 150000000.0
#define SCHEDULER_FREQ_HZ 1000.0
#define PRESCALER  (OSC_FREQ / SCHEDULER_FREQ_HZ / 65536 + 1)
#define TIMER_FREQ_HZ  (SCHEDULER_FREQ_HZ / PRESCALER)
#define TICKS_PER_PERIOD  (uint64_t)(TIMER_FREQ_HZ / SCHEDULER_FREQ_HZ)

//BUTTON
#define INTERFACEBUTTON_READ_FREQ  200.0
#define INTERFACEBUTTON_NECESSARY_BUTTON_READS 5
#define INTERFACEBUTTON_VALUE_IF_PRESSED  0
#define INTERFACEBUTTON_VALUE_IF_UNPRESSED 1

//ENCODER
#define INTERFACEENCODER_READ_FREQ  20.0
#define INTERFACEENCODER_NECESSARY_BUTTON_READS 5
#define INTERFACEENCODER_VALUE_IF_PRESSED  0
#define INTERFACEENCODER_VALUE_IF_UNPRESSED 1

#define PROCESSUS_AFFCHAGE_REFRESH_RATE_MS 100

/* Variables ---------------------------------------------------------------- */
extern int screenval;

/* Prototypes ----------------------------------------------------------------*/
int64_t alarm_callback(alarm_id_t id, void *user_data);
void do_nothing(void);
 #endif