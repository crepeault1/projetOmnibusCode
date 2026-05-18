/**
 ******************************************************************************
 * @file           : process_rotary_encoder_actions.c
 * @brief          :
 * @author         : Samuel Crépeault
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
#include "process_rotary_encoder_actions.h"
#include "interface_rotary_encoder.h"
#include "interface_buttons.h"
#include "service_scheduler.h"

// Debug
#include "frame_assembly.h"

/* Variables -----------------------------------------------------------------*/
PROCESS_ENCODER_ACTIONS process_encoder_actions;
unsigned char counter = 0;
/* Fonctions -----------------------------------------------------------------*/

//=============================================================================
void process_encoder_actions_init(void)
//
//
//=============================================================================
{
  scheduler_phase_array[PHASE_PROCESS_ROTARY_ENC] = process_encoder_actions_run;
  process_encoder_actions.encoder_button_press_callback = do_nothing;
  process_encoder_actions.encoder_button_release_callback = do_nothing;
  process_encoder_actions.encoder_clockwise_callback = do_nothing;
  process_encoder_actions.encoder_counterclockwise_callback = do_nothing;
}

//=============================================================================
void process_encoder_actions_run(void)
//
//=============================================================================
{
  if (interface_encoder.button_information == INFORMATION_AVAILABLE)
  {
    interface_encoder.button_information = INFORMATION_HANDLED;
    if (interface_encoder.button_state == INTERFACEBUTTON_HELD)
    {
      process_encoder_actions.encoder_button_press_callback();
    }
    else
    {
      process_encoder_actions.encoder_button_release_callback();
    }
  }

  if (interface_encoder.encoder_information == INFORMATION_AVAILABLE)
  {
    interface_encoder.encoder_information = INFORMATION_HANDLED;
    if (interface_encoder.encoder_direction == CLOCKWISE)
    {
      process_encoder_actions.encoder_clockwise_callback();
    }
    else
    {
      process_encoder_actions.encoder_counterclockwise_callback();
    }
  }
}