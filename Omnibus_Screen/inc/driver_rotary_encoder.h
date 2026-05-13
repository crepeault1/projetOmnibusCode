#ifndef DRIVERENCODER_H
#define DRIVERENCODER_H
/**
 ******************************************************************************
 * @file           : driver_rotary_encoder.h
 * @brief          :
 * @author         : Samuel Crepeault
 *
 ******************************************************************************
 * @details
 * ENCODER - GPIO - PIN - FUNCTION
 *
 * ENC_A   - GP4  -  6  - Clockwise/counterclockwise
 * ENC_B   - GP5  -  7  - Clockwise/counterclockwise
 * ENC_BUT - GP6  -  9  - None yet
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/

/* Defines -------------------------------------------------------------------*/

/* Variables ---------------------------------------------------------------- */
typedef struct
{
  unsigned long count;
  unsigned char information;
} DRIVER_ENCODER;

extern DRIVER_ENCODER driver_encoder;
/* Functions -----------------------------------------------------------------*/
void driver_encoder_init(void);
uint8_t driver_encoder_read(void);
bool driver_encoder_button_read(void);

static void encoder_gpio_callback(uint gpio, uint32_t events);

#endif