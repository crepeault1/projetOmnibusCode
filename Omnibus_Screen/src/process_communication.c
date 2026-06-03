/**
 ******************************************************************************
 * @file           : process_communication.c
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
#include <string.h>
#include <stdlib.h>

#include "main.h"
#include "process_communication.h"
#include "service_UART.h"
#include "service_scheduler.h"
#include "data_config.h"

#include "process_clock.h"
#include "process_UI_refactored.h"

#include "frame_assembly.h" //DEBUG

/* Defines -------------------------------------------------------------------*/
#define PREFIX_LEN 6
#define FRAME_LEN 24

#define ACTION_NUMBER 11
#define ACTION_CURRENT_TIME 0
#define ACTION_STOP_VALIDITY 1
#define ACTION_LINE_VALIDITY 2
#define ACTION_NUM_LINES_ON_STOP 3
#define ACTION_LINE_ID 4
#define ACTION_DIRECTIONS_ON_LINE 5
#define ACTION_NUM_STOPS_ON_LINE 6
#define ACTION_STOP_ID 7
#define ACTION_DONE 8
#define ACTION_REMAINING_TIME1 9
#define ACTION_REMAINING_TIME2 10

#define EXIT_CLEAR true
#define EXIT_BLOCK false

#define NO_PASSAGE 9999
/* Variables -----------------------------------------------------------------*/
const char *check_stop_str = "(SCHK_";
const char *check_line_str = "(LCHK_";
const char *get_lines_str = "(LGET_";
const char *ack_line_quantity_str = "(LACK_";
const char *set_direction_str = "(DSET_";
const char *get_directions_str = "(DGET_";
const char *get_stops_str = "(SGET_";
const char *ack_stop_quantity_str = "(SACK_";
const char *get_times_str = "(WAIT_";
const char *reset_str = "(RESET";
const char *end_of_frame_str = ")";

static const INCOMING_FRAME_TYPE frame_header_lut[] = {
    {"TIME", ACTION_CURRENT_TIME},
    {"SVAL", ACTION_STOP_VALIDITY},
    {"LVAL", ACTION_LINE_VALIDITY},
    {"LNBR", ACTION_NUM_LINES_ON_STOP},
    {"LIDN", ACTION_LINE_ID},
    {"DIRE", ACTION_DIRECTIONS_ON_LINE},
    {"SNBR", ACTION_NUM_STOPS_ON_LINE},
    {"SIDN", ACTION_STOP_ID},
    {"DONE", ACTION_DONE},
    {"REM1", ACTION_REMAINING_TIME1},
    {"REM2", ACTION_REMAINING_TIME2}};

uint8_t incoming_frame[24];
char outbound_frame[16];
uint8_t process_communication_outbound_code;
bool process_communication_exit_block;
/* Functions -----------------------------------------------------------------*/

//==============================================================================
void process_communication_init(void)
//
//==============================================================================
{
  scheduler_phase_array[PHASE_PROCESS_COMMUNICATION_IN] = process_communication_read;
  scheduler_phase_array[PHASE_PROCESS_COMMUNICATION_OUT] = process_communication_write;
  process_communication_outbound_code = NOTHING_TO_DO;
  process_communication_exit_block = false;
}

//==============================================================================
void process_communication_read(void)
//
//==============================================================================
{
  if (uart_service_read(incoming_frame, 0))
  {
    for (int i = 0; i < ACTION_NUMBER; i++)
    {
      if (memcmp(&incoming_frame[1], frame_header_lut[i].header, 4) == 0)
      {
        process_communication_interpret_frame(frame_header_lut[i].action);
      }
    }
  }
}

//==============================================================================
void process_communication_write(void)
// #: stop number
//&: line number
//*: generic number
// É: generic character
//+: padding character
//==============================================================================
{
  char line_stop_buffer[6];
  // Writing
  switch (process_communication_outbound_code)
  {
  case NOTHING_TO_DO:
    return;

  case CHECK_STOP:                                       //(SCHK_####)
    memcpy(&outbound_frame, check_stop_str, PREFIX_LEN); // Copy prefix
    snprintf(line_stop_buffer, 6, "%04d", data_config_user_setup.selected_stop);
    memcpy(&outbound_frame[6], line_stop_buffer, 4);  // Copy string into frame
    memcpy(&outbound_frame[10], end_of_frame_str, 1); // Copy end-of-frame charact.
    break;

  case CHECK_LINE: //(LCHK_&&&)
    memcpy(&outbound_frame, check_line_str, PREFIX_LEN);
    snprintf(line_stop_buffer, 6, "%03d", data_config_user_setup.selected_line);
    memcpy(&outbound_frame[6], line_stop_buffer, 3);
    memcpy(&outbound_frame[9], end_of_frame_str, 1);
    break;

  case GET_LINES: //(LGET_####)
    memcpy(&outbound_frame, get_lines_str, PREFIX_LEN);
    snprintf(line_stop_buffer, 6, "%04d", data_config_user_setup.selected_stop);
    memcpy(&outbound_frame[6], line_stop_buffer, 4);
    memcpy(&outbound_frame[10], end_of_frame_str, 1);
    break;

  case ACK_LINE_QUANTITY: //(LACK_***)
    memcpy(&outbound_frame, ack_line_quantity_str, PREFIX_LEN);
    snprintf(line_stop_buffer, 6, "%03d", data_config_bus_data_dummy.number_of_lines);
    memcpy(&outbound_frame[6], line_stop_buffer, 3);
    memcpy(&outbound_frame[9], end_of_frame_str, 1);
    break;

  case GET_DIRECTIONS: //(DGET_&&&)
    memcpy(&outbound_frame, get_directions_str, PREFIX_LEN);
    snprintf(line_stop_buffer, 6, "%03d", data_config_user_setup.selected_line);
    memcpy(&outbound_frame[6], line_stop_buffer, 3);
    memcpy(&outbound_frame[9], end_of_frame_str, 1);
    break;

  case GET_STOPS: //(SGET_&&&*)
    memcpy(&outbound_frame, get_stops_str, PREFIX_LEN);
    snprintf(line_stop_buffer, 6, "%03d", data_config_user_setup.selected_line);
    memcpy(&outbound_frame[6], line_stop_buffer, 3);
    snprintf(line_stop_buffer, 6, "%01d", data_config_user_setup.selected_direction);
    memcpy(&outbound_frame[9], line_stop_buffer, 1);
    memcpy(&outbound_frame[10], end_of_frame_str, 1);
    break;

  case ACK_STOP_QUANTITY: //(SACK_***)
    memcpy(&outbound_frame, ack_stop_quantity_str, PREFIX_LEN);
    snprintf(line_stop_buffer, 6, "%03d", data_config_bus_data_dummy.number_of_stops);
    memcpy(&outbound_frame[6], line_stop_buffer, 3);
    memcpy(&outbound_frame[9], end_of_frame_str, 1);
    break;

  case WAIT_TIMES: //(WAIT_####_&&&)
    memcpy(&outbound_frame, get_times_str, PREFIX_LEN);
    snprintf(line_stop_buffer, 6, "%04d", data_config_user_setup.selected_stop);
    memcpy(&outbound_frame[6], line_stop_buffer, 4);
    outbound_frame[10] = '_';
    snprintf(line_stop_buffer, 6, "%03d", data_config_user_setup.selected_line);
    memcpy(&outbound_frame[11], line_stop_buffer, 3);
    memcpy(&outbound_frame[14], end_of_frame_str, 1);
    break;

  case RESET:
    memcpy(&outbound_frame, reset_str, PREFIX_LEN);
    memcpy(&outbound_frame[6], end_of_frame_str, 1);
    break;

  default:
    break;
  }

  uart_service_write(outbound_frame, 5); // Buffer and timeout val.
  memset(line_stop_buffer, 0, 6);        // Reset buffer
  memset(outbound_frame, 0, 16);
  process_communication_outbound_code = NOTHING_TO_DO;
}

//==============================================================================
void process_communication_interpret_frame(int action)
//
//==============================================================================
{
  uint32_t temp = 0;

  switch (action)
  {
  case ACTION_CURRENT_TIME:
    temp = atoi(&incoming_frame[PREFIX_LEN]);
    process_clock.epoch = temp - (4 * 3600); // UTC conversion
    process_clock.update = true;
    break;

  // By stop
  case ACTION_STOP_VALIDITY:
    if (incoming_frame[10] == 'G')
    {
      process_communication_exit_block = EXIT_CLEAR;
      ui_request_transition(&state, 48, UI_NAV_AUTO);
      process_communication_outbound_code = GET_LINES;
    }
    else if (incoming_frame[10] == 'B')
    {
      write_string_to_array("Arret invalide", 7, 3, 0, 14);
      process_communication_exit_block = EXIT_BLOCK;
    }
    break;
  case ACTION_NUM_LINES_ON_STOP:
    data_config_bus_data_dummy.number_of_lines = atoi(&incoming_frame[PREFIX_LEN]);
    process_communication_outbound_code = ACK_LINE_QUANTITY;
    break;
  case ACTION_LINE_ID:
    data_config_bus_data_dummy.line_list[data_config_bus_data_dummy.line_counter] = atoi(&incoming_frame[PREFIX_LEN]);
    data_config_bus_data_dummy.line_counter++;
        if (data_config_bus_data_dummy.line_counter >= data_config_bus_data_dummy.number_of_lines)
    {
      arrival_48(&state);
    }
    break;

  // By line
  case ACTION_LINE_VALIDITY:
    if (incoming_frame[9] == 'G')
    {
      process_communication_outbound_code = GET_DIRECTIONS;
    }
    else if (incoming_frame[9] == 'B')
    {
      write_string_to_array("Ligne invalide", 7, 3, 0, 14);
      process_communication_exit_block = EXIT_BLOCK;
    }
    break;
  case ACTION_DIRECTIONS_ON_LINE:
    memcpy(data_config_bus_data_dummy.directions[data_config_bus_data_dummy.direction_counter], &incoming_frame[PREFIX_LEN], 18);
    data_config_bus_data_dummy.directions[data_config_bus_data_dummy.direction_counter][17] = ' ';
    data_config_bus_data_dummy.direction_counter++;
    if (data_config_bus_data_dummy.direction_counter >= 2)
    {
      process_communication_exit_block = EXIT_CLEAR;
      ui_request_transition(&state, 45, UI_NAV_AUTO);
    }

    break;
  case ACTION_NUM_STOPS_ON_LINE:
    data_config_bus_data_dummy.number_of_stops = atoi(&incoming_frame[PREFIX_LEN]);
    process_communication_outbound_code = ACK_STOP_QUANTITY;
    break;
  case ACTION_STOP_ID:
    data_config_bus_data_dummy.timetable_stop_list[data_config_bus_data_dummy.stop_counter] = atoi(&incoming_frame[PREFIX_LEN]);
    memcpy(data_config_bus_data_dummy.timetable_stop_names[data_config_bus_data_dummy.stop_counter], &incoming_frame[11], 11);
    data_config_bus_data_dummy.stop_counter++;
    if (data_config_bus_data_dummy.stop_counter >= data_config_bus_data_dummy.number_of_stops)
    {
      arrival_46(&state);
    }
    break;

  case ACTION_REMAINING_TIME1:
    uint32_t stop_line = atoi(&incoming_frame[PREFIX_LEN]);
    uint16_t stop = stop_line / 1000;
    uint16_t line = stop_line % 1000;
    uint16_t time1 = atoi(&incoming_frame[14]);
    uint16_t time2 = atoi(&incoming_frame[19]);
    uint8_t display_index;

    //DEBUG
    //write_number_to_array(data_config_bus_data_dummy.timetable_stop_list[0], 7, 0, 4);
    //write_string_to_array(data_config_bus_data_dummy.timetable_stop_names[0], 7, 6, 0, 10);

    for (int i = 0; i < MAX_STOPS; i++)
    {
      if ((stop == data_config_user_setup.added_stops_and_lines[i][0]) && (line == data_config_user_setup.added_stops_and_lines[i][1]))
      {
        display_index = i;
      }
    }

    if (time1 != NO_PASSAGE)
    {
      data_config_bus_data_dummy.minutes_until_passage[display_index][0] = time1;
      data_config_bus_data_dummy.minutes_until_passage[display_index][1] = time2;
    }
    else
    {
      data_config_bus_data_dummy.minutes_until_passage[display_index][0] = time1;
      data_config_bus_data_dummy.minutes_until_passage[display_index][1] = time2;
      // hide lingering zero when replacing 00m 00m 00m with 99h99 99h99
      write_character_to_array(' ', 2 + display_index, 15);
    }
    break;

  case ACTION_REMAINING_TIME2:
    stop_line = atoi(&incoming_frame[PREFIX_LEN]);
    stop = stop_line / 1000;
    line = stop_line % 1000;
    uint16_t time3 = atoi(&incoming_frame[14]);

    for (int i = 0; i < MAX_STOPS; i++)
    {
      if ((stop == data_config_user_setup.added_stops_and_lines[i][0]) && (line == data_config_user_setup.added_stops_and_lines[i][1]))
      {
        display_index = i;
      }
    }
    if (time3 != NO_PASSAGE)
    {
      data_config_bus_data_dummy.minutes_until_passage[display_index][2] = time3;
    }

    break;
  case ACTION_DONE:
    break;
  }
}