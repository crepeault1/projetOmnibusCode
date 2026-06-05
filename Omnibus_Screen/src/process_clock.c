/**
 ******************************************************************************
 * @file           : process_clock.c
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
#include "process_clock.h"
#include "service_scheduler.h"
#include "frame_assembly.h"
#include "data_config.h"

/* Defines -------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/
PROCESS_CLOCK process_clock;
/* Functions -----------------------------------------------------------------*/

void process_clock_init(void)
{
    scheduler_phase_array[PHASE_PROCESS_CLOCK] = process_clock_handle;
    process_clock.display_on = true;
}

void process_clock_handle(void)
{
    if (process_clock.update)
    {
        process_clock.update = false;

        char time_string[8];
        char seconds = process_clock.epoch % 60;
        char minutes = (process_clock.epoch / 60) % 60;
        char hours = (process_clock.epoch / 3600) % 24;

        uint16_t minutes_now = hours * 60 + minutes;
        uint16_t minutes_start = data_config_user_setup.desired_start_hour * 60 + data_config_user_setup.desired_start_minute;
        uint16_t minutes_stop = data_config_user_setup.desired_stop_hour * 60 + data_config_user_setup.desired_stop_minute;

        // put in exit 63?
        bool window_crosses_midnight = (minutes_start > minutes_stop);
        bool in_window = window_crosses_midnight
                             ? (minutes_now >= minutes_start || minutes_now < minutes_stop)
                             : (minutes_now >= minutes_start && minutes_now < minutes_stop);

        if (process_clock.epoch < process_clock.awake_until_epoch)
        {
            process_clock.display_on = true;
        }
        else if (data_config_user_setup.desired_display_mode == ALWAYS_ON)
        {
            process_clock.display_on = true;
        }
        else if (data_config_user_setup.desired_display_mode == PROGRAMMED_TIME)
        {
            if (in_window)
            {
                process_clock.display_on = true;
            }
            else
            {
                process_clock.display_on = false;
            }
        }

        if (data_config_user_setup.desired_time_format == T_12)
        {
            if (hours <= 11)
            {
                write_string_to_array("AM", 0, 9, 0, 2);
            }
            else
            {
                write_string_to_array("PM", 0, 9, 0, 2);
            }
            if (hours >= 13)
                hours -= 12;
        }
        snprintf(time_string, 3, "%02d", hours);
        snprintf(&time_string[3], 3, "%02d", minutes);
        snprintf(&time_string[6], 3, "%02d", seconds);
        time_string[2] = ':';
        time_string[5] = ':';
        write_string_to_array(time_string, 0, 0, 0, 8);
    }
}