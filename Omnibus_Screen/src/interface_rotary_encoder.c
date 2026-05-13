/**
 ******************************************************************************
 * @file           : interface_rotary_encoder.c
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
#include "driver_rotary_encoder.h"
#include "service_scheduler.h"
#include "interface_rotary_encoder.h"

// Debug
#include "frame_assembly.h"
/* Defines -------------------------------------------------------------------*/
#define INTERFACEENCODER_COUNT_BEFORE_READING (SCHEDULER_FREQ_HZ / INTERFACEBUTTON_READ_FREQ)
#define INTERFACEENCODER_INITIAL_DEBOUNCE_VAL (INTERFACEBUTTON_NECESSARY_BUTTON_READS / 2)

/* Variables -----------------------------------------------------------------*/
INTERFACE_ENCODER interface_encoder;
const int8_t direction_table[4][4] = {
    {0, 1, 0, -1},
    {-1, 0, 1, 0},
    {0, -1, 0, 1},
    {1, 0, -1, 0}};
/* Fonctions -----------------------------------------------------------------*/

//=============================================================================
void interface_encoder_init(void)
//
//=============================================================================
{
    interface_encoder.encoder_last_count = driver_encoder.count;
    interface_encoder.encoder_direction = UNKNOWN;
    interface_encoder.encoder_information = INFORMATION_HANDLED;
    interface_encoder.encoder_read_counter = 0;

    interface_encoder.button_state = INTERFACEENCODER_RELEASED;
    interface_encoder.button_information = INFORMATION_HANDLED;
    interface_encoder.button_debounce_counter = INTERFACEENCODER_INITIAL_DEBOUNCE_VAL;
    interface_encoder.button_read_counter = 0;

    scheduler_phase_array[PHASE_INTERFACE_ROTARY_ENC_BUTTON] = interface_encoder_button_run;
    scheduler_phase_array[PHASE_INTERFACE_ROTARY_ENC] = interface_encoder_run;
}

//=============================================================================
void interface_encoder_button_run(void)
//
// BUTTON BUTTON BUTTON
//=============================================================================
{
    interface_encoder.button_read_counter++;
    if (interface_encoder.button_read_counter < INTERFACEENCODER_COUNT_BEFORE_READING)
    {
        return;
    }
    interface_encoder.button_read_counter = 0;
    if (driver_encoder_button_read() == INTERFACEENCODER_VALUE_IF_PRESSED)
    {
        if (interface_encoder.button_debounce_counter == INTERFACEENCODER_NECESSARY_BUTTON_READS)
        {
            return;
        }
        interface_encoder.button_debounce_counter++;
        if (interface_encoder.button_debounce_counter < INTERFACEENCODER_NECESSARY_BUTTON_READS)
        {
            return;
        }
        interface_encoder.button_state = INTERFACEENCODER_HELD;
        interface_encoder.button_information = INFORMATION_AVAILABLE;
        return;
    }
    if (interface_encoder.button_debounce_counter == 0)
    {
        return;
    }
    interface_encoder.button_debounce_counter--;
    if (interface_encoder.button_debounce_counter > 0)
    {
        return;
    }
    interface_encoder.button_state = INTERFACEENCODER_RELEASED;
    interface_encoder.button_information = INFORMATION_AVAILABLE;
}

//=============================================================================
void interface_encoder_run(void)
//
// ENCODER ENCODER ENCODER
//=============================================================================
{
    interface_encoder.encoder_read_counter++;
    if (interface_encoder.encoder_read_counter < INTERFACEENCODER_COUNT_BEFORE_READING)
    {
        return;
    }
    interface_encoder.encoder_read_counter = 0;
    
    if (driver_encoder.count > interface_encoder.encoder_last_count)
    {
        interface_encoder.encoder_direction = CLOCKWISE;
        interface_encoder.encoder_information = INFORMATION_AVAILABLE;
        interface_encoder.encoder_last_count = driver_encoder.count;
        driver_encoder.information = INFORMATION_HANDLED;
    }

    if (driver_encoder.count < interface_encoder.encoder_last_count)
    {
        interface_encoder.encoder_direction = COUNTERCLOCKWISE;
        interface_encoder.encoder_information = INFORMATION_AVAILABLE;
        interface_encoder.encoder_last_count = driver_encoder.count;
        driver_encoder.information = INFORMATION_HANDLED;
    }

}
