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

/* Defines -------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/
PROCESS_CLOCK process_clock;

/* Functions -----------------------------------------------------------------*/

void process_clock_init(void)
{
    scheduler_phase_array[PHASE_PROCESS_CLOCK] = process_clock_handle;
}

void process_clock_handle(void)
{
    if(process_clock.update)
    {
        process_clock.update = false;

        char time_string[8];
        char seconds = process_clock.epoch % 60;
        char minutes = (process_clock.epoch / 60) % 60;
        char hours = (process_clock.epoch / 3600) % 24;

        snprintf(time_string, 3, "%02d", hours);
        snprintf(&time_string[3], 3, "%02d", minutes);
        snprintf(&time_string[6], 3, "%02d", seconds);
        time_string[2] = ':';
        time_string[5] = ':';
        write_string_to_array(time_string, 0, 0, 0, 8);
    }
}