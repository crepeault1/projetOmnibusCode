/**
 ******************************************************************************
 * @file           : driver_buttons.c
 * @brief          :
 * @author         : Samuel Crepeault
 *
 ******************************************************************************
 * @details
 * BUT - GPIO - PIN - FUNCTION
 * 
 * SW1 - GP4  -  6  - Select
 * SW2 - GP5  -  7  - Menu
 * SW3 - GP6  -  9  - Wake
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "driver_buttons.h"

/* Variables -----------------------------------------------------------------*/

/* Fonctions -----------------------------------------------------------------*/
void driver_buttons_init(void)
{
  gpio_init(SW1_PIN);
  gpio_set_dir(SW1_PIN, GPIO_IN);
  gpio_pull_up(SW1_PIN);

  gpio_init(SW2_PIN);
  gpio_set_dir(SW2_PIN, GPIO_IN);
  gpio_pull_up(SW2_PIN);

  gpio_init(SW3_PIN);
  gpio_set_dir(SW3_PIN, GPIO_IN);
  gpio_pull_up(SW3_PIN);
}

bool driverIOB1_read(void)
{
  return gpio_get(SW1_PIN);
}

bool driverIOB2_read(void)
{
  return gpio_get(SW2_PIN);
}

bool driverIOB3_read(void)
{
  return gpio_get(SW3_PIN);
}