#ifndef MAIN_H
#define MAIN_H
/**
 ******************************************************************************
 * @file           : Omnibus_Screen.c
 * @brief          : Main program body
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
#include "hardware/timer.h"
#include "hardware/uart.h"
/* Defines -------------------------------------------------------------------*/

/* Variables ---------------------------------------------------------------- */

/* Fonctions -----------------------------------------------------------------*/
int64_t alarm_callback(alarm_id_t id, void *user_data);

 #endif