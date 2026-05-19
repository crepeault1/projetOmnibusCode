#ifndef DATACONFIG_H
#define DATACONFIG_H
/**
  ******************************************************************************
  * @file           : data_config.h
  * @brief          : header of config variables file
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

/* Defines -------------------------------------------------------------------*/

/* Variables ---------------------------------------------------------------- */
typedef enum {
    ALWAYS_ON,
    PROGRAMMED_TIME,
    WAKE_WITH_BUTTON
} E_DATA_CONFIG_DISPLAY_MODE;

typedef enum {
    T_12,
    T_24
} E_DATA_CONFIG_TIME_FORMAT;

typedef struct
{
    uint8_t current_line_numbers[3];
    uint8_t current_stop_numbers[4];
    uint8_t current_hour_number;
    uint8_t current_minute_number;
    uint16_t current_connection_word[100];

    uint16_t desired_start_time;
    uint16_t desired_stop_time;
    uint16_t added_stops[6];

    E_DATA_CONFIG_DISPLAY_MODE desired_display_mode;
    E_DATA_CONFIG_TIME_FORMAT desired_time_format;
} DATA_CONFIG_USER_SETUP;
/* Prototypes ----------------------------------------------------------------*/

#endif