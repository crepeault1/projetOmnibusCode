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
 *  UART0 TX - GP0 -  1  |        USB       |  40 - VBUS
 *  UART0 RX - GP1 -  2  |                  |  39 - VSYS
 *             GND -  3  |                  |  38 - GND
 *   I2C SDA - GP2 -  4  |                  |  37 - 3V3_EN
 *   I2C SCL - GP3 -  5  |                  |  36 - 3V3_OUT
 *       SW1 - GP4 -  6  |                  |  35 - ADC_VREF
 *       SW2 - GP5 -  7  |                  |  34 - GP28 - ENC_BUT
 *             GND -  8  |                  |  33 - GND
 *       SW3 - GP6 -  9  |                  |  32 - GP27 - ENC_B
 *   BUF2_EN - GP7 - 10  |                  |  31 - GP26 - ENC_A
 *         A - GP8 - 11  |                  |  30 - RUN
 *         B - GP9 - 12  |                  |  29 - GP22 - B1
 *             GND - 13  |                  |  28 - GND
 *        C - GP10 - 14  |                  |  27 - GP21 - G1
 *        D - GP11 - 15  |                  |  26 - GP20 - R1
 *        E - GP12 - 16  |                  |  25 - GP19 - B0
 *      CLK - GP13 - 17  |                  |  24 - GP18 - G0
 *             GND - 18  |                  |  23 - GND
 *      LAT - GP14 - 19  |                  |  22 - GP17 - R0
 *       OE - GP15 - 20  |__________________|  21 - GP16 - BUF1_EN
 *
 * @attention
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "data_font_5x7.h"
#include "frame_assembly.h"

//Data
#include "data_config.h"
#include "data_UI_refactored.h"

//Services
#include "service_scheduler.h"
#include "service_UART.h"

//Drivers
#include "driver_buttons.h"
#include "driver_UART.h"
#include "driver_rotary_encoder.h"
#include "driver_timer.h"
#include "driver_HUB75E.h"

//Interfaces
#include "interface_buttons.h"
#include "interface_rotary_encoder.h"

//Processes
#include "process_button_actions.h"
#include "process_rotary_encoder_actions.h"
#include "process_UI_refactored.h"
#include "process_communication.h"
#include "process_clock.h"

/* Defines -------------------------------------------------------------------*/
#define WIDTH 128
#define HEIGHT 64

/* Variables ---------------------------------------------------------------- */
int screenval = 0;

/* Fonctions -----------------------------------------------------------------*/

//==============================================================================
void init(void)
//
// Initialisation des divers périphériques.
//==============================================================================
{
    stdio_init_all();

    //Data
    data_config_init();
    data_UI_init();
    
    //Services
    service_scheduler_init();
    service_UART_init(uart0, UART_TX_PIN, UART_RX_PIN, BAUD_RATE);

    //Drivers
    driver_timer_init();
    driver_HUB75E_init();
    driver_buttons_init();
    driver_encoder_init();

    //Interfaces
    interface_button1_init();
    interface_button2_init();
    interface_button3_init();
    interface_encoder_init();

    //Processes
    process_button_actions_init();
    process_encoder_actions_init();
    process_UI_init();
    frame_assembly_init();
    process_communication_init();
    process_clock_init();

    //I2C init
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
}

//==============================================================================
int main()
//==============================================================================
{
    uint8_t test_frame[24] = {"(SCHK_6453)+++++++++++++"};
    int counter = 0;
    init();
    //character_buffer_to_pixel_buffer(frame_menu);
    //pixel_buffer_to_frame_buffer();
    while (1)
    {
        if (systick_flag)
        {
            systick_flag = false;
            service_scheduler_run();
            driver_HUB75E_run();
            counter++;
            if(counter > 100) 
            {
                counter = 0;
                //uart_service_write(test_frame, 0);
            }
        }
    }
}

//==============================================================================
void do_nothing(void)
//
// Placeholder for function pointer array, before initialisation.
//==============================================================================
{
}