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
// Fonction d'initialisation des valeurs de la struct processusBouton
//=============================================================================
{
  scheduler_phase_array[PHASE_PROCESS_ROTARY_ENC] = process_encoder_actions_run;
}

//=============================================================================
void process_encoder_actions_run(void)
//
//=============================================================================
{
  if (interface_encoder.button_information == INFORMATION_AVAILABLE)
  {
    interface_encoder.button_information = INFORMATION_HANDLED;
    if (interface_encoder.button_state == INTERFACEBUTTON_VALUE_IF_PRESSED)
    {
    }
    else
    {
    }
  }

  if (interface_encoder.encoder_information == INFORMATION_AVAILABLE)
  {
    interface_encoder.encoder_information = INFORMATION_HANDLED;
    if (interface_encoder.encoder_direction == CLOCKWISE)
    {
    }
    else
    {
    }
  }
}