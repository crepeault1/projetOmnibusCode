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

#include "frame_assembly.h" //DEBUG

/* Defines -------------------------------------------------------------------*/
#define PREFIX_LEN 6
#define FRAME_LEN 24

#define ACTION_CURRENT_TIME 0
#define ACTION_STOP_VALIDITY 1
#define ACTION_LINE_VALIDITY 2
#define ACTION_NUM_LINES_ON_STOP 3
#define ACTION_LINE_ID 4
#define ACTION_DIRECTIONS_ON_LINE 5
#define ACTION_NUM_STOPS_ON_LINE 6
#define ACTION_STOP_ID 7
#define ACTION_DONE 8
#define ACTION_REMAINING_TIME 9

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
    {"REMN", ACTION_REMAINING_TIME}};

uint8_t incoming_frame[24];
char outbound_frame[16];
uint8_t process_communication_outbound_code;
/* Functions -----------------------------------------------------------------*/

//==============================================================================
void process_communication_init(void)
//
//==============================================================================
{
  scheduler_phase_array[PHASE_PROCESS_COMMUNICATION_IN] = process_communication_read;
  scheduler_phase_array[PHASE_PROCESS_COMMUNICATION_OUT] = process_communication_write;
  process_communication_outbound_code = NOTHING_TO_DO;
}

//==============================================================================
void process_communication_read(void)
//
//==============================================================================
{
  if (uart_service_read(incoming_frame, 0))
  {
    for (int i = 0; i < 10; i++)
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
//
//==============================================================================
{
  int number = 15;
  char line_stop_buffer[6];
  // Writing
  switch (process_communication_outbound_code)
  {
  case NOTHING_TO_DO:
    return;

  case CHECK_STOP:
    memcpy(&outbound_frame, check_stop_str, PREFIX_LEN); // Copy prefix
    snprintf(line_stop_buffer, 6, "%04d", data_config_user_setup.selected_stop);
    memcpy(&outbound_frame[6], line_stop_buffer, 4);  // Copy string into frame
    memcpy(&outbound_frame[10], end_of_frame_str, 1); // Copy end-of-frame charact.
    break;

  case CHECK_LINE:
    memcpy(&outbound_frame, check_line_str, PREFIX_LEN);
    snprintf(line_stop_buffer, 6, "%03d", data_config_user_setup.selected_line);
    memcpy(&outbound_frame[6], line_stop_buffer, 3);
    memcpy(&outbound_frame[9], end_of_frame_str, 1);
    break;

  case GET_LINES:
    memcpy(&outbound_frame, get_lines_str, PREFIX_LEN);
    snprintf(line_stop_buffer, 6, "%04d", data_config_user_setup.selected_stop);
    memcpy(&outbound_frame[6], line_stop_buffer, 4);
    memcpy(&outbound_frame[10], end_of_frame_str, 1);
    break;

  case ACK_LINE_QUANTITY:
    memcpy(&outbound_frame, ack_line_quantity_str, PREFIX_LEN);
    snprintf(line_stop_buffer, 6, "%03d", data_config_bus_data_dummy.number_of_lines);
    memcpy(&outbound_frame[6], line_stop_buffer, 3);
    memcpy(&outbound_frame[9], end_of_frame_str, 1);
    break;

  case GET_DIRECTIONS:
    memcpy(&outbound_frame, get_directions_str, PREFIX_LEN);
    snprintf(line_stop_buffer, 6, "%03d", data_config_user_setup.selected_line);
    memcpy(&outbound_frame[6], line_stop_buffer, 3);
    memcpy(&outbound_frame[9], end_of_frame_str, 1);
    break;

  case SET_DIRECTION:
    memcpy(&outbound_frame, set_direction_str, PREFIX_LEN);
    snprintf(line_stop_buffer, 6, "%01d", data_config_user_setup.selected_direction);
    memcpy(&outbound_frame[6], line_stop_buffer, 3);
    memcpy(&outbound_frame[7], end_of_frame_str, 1);
    break;

  case GET_STOPS:
    memcpy(&outbound_frame, get_stops_str, PREFIX_LEN);
    snprintf(line_stop_buffer, 6, "%03d", data_config_user_setup.selected_line);
    memcpy(&outbound_frame[6], line_stop_buffer, 3);
    memcpy(&outbound_frame[9], end_of_frame_str, 1);
    break;

  case ACK_STOP_QUANTITY:
    memcpy(&outbound_frame, ack_stop_quantity_str, PREFIX_LEN);
    itoa(number, line_stop_buffer, 10); // CHANGE
    memcpy(&outbound_frame[6], line_stop_buffer, 3);
    memcpy(&outbound_frame[9], end_of_frame_str, 1);
    break;

  case WAIT_TIMES:
    memcpy(&outbound_frame, get_times_str, PREFIX_LEN);
    snprintf(line_stop_buffer, 6, "%04d", data_config_user_setup.selected_stop);
    memcpy(&outbound_frame[6], line_stop_buffer, 4);
    outbound_frame[10] = '_';
    snprintf(line_stop_buffer, 6, "%03d", data_config_user_setup.selected_line);
    memcpy(&outbound_frame[11], line_stop_buffer, 3);
    memcpy(&outbound_frame[14], end_of_frame_str, 1);
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
      write_string_to_array("SG", 0, 19, 0, 2);
      process_communication_outbound_code = GET_LINES;
    }
    else if (incoming_frame[10] == 'B')
    {
      write_string_to_array("SB", 0, 19, 0, 2);
    }
    break;
  case ACTION_NUM_LINES_ON_STOP:
    data_config_bus_data_dummy.number_of_lines = atoi(&incoming_frame[PREFIX_LEN]);
    process_communication_outbound_code = ACK_LINE_QUANTITY;
    break;
  case ACTION_LINE_ID:
    data_config_bus_data_dummy.line_list[data_config_bus_data_dummy.line_counter] = atoi(&incoming_frame[PREFIX_LEN]);
    data_config_bus_data_dummy.line_counter++;
    break;

  // By line
  case ACTION_LINE_VALIDITY:
    if (incoming_frame[9] == 'G')
    {
      write_string_to_array("LG", 0, 19, 0, 2);
      process_communication_outbound_code = GET_DIRECTIONS;
    }
    else if (incoming_frame[9] == 'B')
    {
      write_string_to_array("LB", 0, 19, 0, 2);
    }
    break;
  case ACTION_DIRECTIONS_ON_LINE:
    memcpy(data_config_bus_data_dummy.directions[data_config_bus_data_dummy.direction_counter], &incoming_frame[PREFIX_LEN], 18);
    data_config_bus_data_dummy.directions[data_config_bus_data_dummy.direction_counter][17] = ' ';
    data_config_bus_data_dummy.direction_counter++;
    break;
  case ACTION_NUM_STOPS_ON_LINE:

    break;
  case ACTION_STOP_ID:
    break;
  case ACTION_DONE:
    break;
  case ACTION_REMAINING_TIME:
    break;
  }
}