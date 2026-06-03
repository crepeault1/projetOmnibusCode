/**
  ******************************************************************************
  * @file           : data_config.c
  * @brief          : Holds data for program operation
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
#include "data_config.h"

/* Defines -------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/
DATA_CONFIG_USER_SETUP data_config_user_setup;
DATA_CONFIG_BUS_DATA data_config_bus_data_dummy;
/* Functions -----------------------------------------------------------------*/
void data_config_init(void)
{
  data_config_user_setup.number_added_stops = 0;
  data_config_user_setup.desired_display_mode = ALWAYS_ON;
  data_config_user_setup.desired_time_format = T_24;
}