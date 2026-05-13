#ifndef DRIVERBUTTONS_H
#define DRIVERBUTTONS_H
/**
  ******************************************************************************
  * @file           : driver_buttons.h
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

/* Defines -------------------------------------------------------------------*/

/* Variables ---------------------------------------------------------------- */

/* Functions -----------------------------------------------------------------*/
void driver_buttons_init(void);

bool driverIOB1_read(void);
bool driverIOB2_read(void);
bool driverIOB3_read(void);

#endif