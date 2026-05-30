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
#define MAX_STOPS 6

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
    uint16_t selected_line;

    uint16_t current_stop_numbers[4];
    uint16_t selected_stop;

    uint8_t selected_direction;

    uint8_t current_hour_number;
    uint8_t current_minute_number;
    uint16_t current_connection_word[100];

    uint16_t desired_start_time;
    uint16_t desired_stop_time;

    uint16_t added_stops_and_lines[MAX_STOPS][2];
    uint8_t added_stops_and_lines_index;
    uint8_t number_added_stops;

    E_DATA_CONFIG_DISPLAY_MODE desired_display_mode;
    E_DATA_CONFIG_TIME_FORMAT desired_time_format;

} DATA_CONFIG_USER_SETUP;



typedef struct
{
    uint8_t  number_of_lines;
    uint8_t  number_of_stops;
    uint16_t timetable_stop_list[100];
    uint8_t  timetable_stop_names[100][12];
    uint8_t stop_counter;
    uint16_t  line_list[150];
    uint8_t line_counter;
    uint8_t  directions[2][18];
    uint8_t direction_counter;
    
    volatile uint16_t minutes_until_passage[MAX_STOPS][3];
} DATA_CONFIG_BUS_DATA;



extern DATA_CONFIG_USER_SETUP data_config_user_setup;
extern DATA_CONFIG_BUS_DATA data_config_bus_data_dummy;
/* Prototypes ----------------------------------------------------------------*/
void data_config_init(void);
#endif