/**
 ******************************************************************************
 * @file           : driver_rotary_encoder.c
 * @brief          :
 * @author         : Samuel Crepeault
 *
 ******************************************************************************
 * @details
 *
 * ENCODER - GPIO -  PIN - FUNCTION
 *
 * ENC_A   - GP26 -  31  - Clockwise/counterclockwise rotation
 * ENC_B   - GP27 -  32  - Clockwise/counterclockwise rotation
 * ENC_BUT - GP28 -  34  - None yet
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "driver_rotary_encoder.h"

/* Variables -----------------------------------------------------------------*/
DRIVER_ENCODER driver_encoder;

/* Fonctions -----------------------------------------------------------------*/
void driver_encoder_init(void)
{
    gpio_init(ENC_A_PIN);
    gpio_set_dir(ENC_A_PIN, GPIO_IN);

    gpio_init(ENC_B_PIN);
    gpio_set_dir(ENC_B_PIN, GPIO_IN);

    gpio_set_irq_enabled_with_callback(
        ENC_A_PIN,
        GPIO_IRQ_EDGE_FALL,
        true,
        &encoder_gpio_callback);

    gpio_set_irq_enabled_with_callback(
        ENC_B_PIN,
        GPIO_IRQ_EDGE_FALL,
        true,
        &encoder_gpio_callback);

    gpio_init(ENC_BUT_PIN);
    gpio_set_dir(ENC_BUT_PIN, GPIO_IN);
    gpio_pull_up(ENC_BUT_PIN);

    driver_encoder.count = 1000000000;
    driver_encoder.information = INFORMATION_HANDLED;
}

static void encoder_gpio_callback(uint gpio, uint32_t events)
{
    if (events & GPIO_IRQ_EDGE_FALL)
    {
        //Counter-clockwise
        if (gpio == ENC_A_PIN)
        {
            if (gpio_get(ENC_B_PIN))
            {
                driver_encoder.count--;
                driver_encoder.information = INFORMATION_AVAILABLE;
            }
        }
        //Clockwise
        else if (gpio == ENC_B_PIN)
        {
            if (gpio_get(ENC_A_PIN))
            {
                driver_encoder.count++;
                driver_encoder.information = INFORMATION_AVAILABLE;
            }
        }
    }
}

bool driver_encoder_button_read(void)
{
    return gpio_get(ENC_BUT_PIN);
}