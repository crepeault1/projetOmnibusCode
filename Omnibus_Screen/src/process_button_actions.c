/**
 ******************************************************************************
 * @file           : Processus_Boutons.c
 * @brief          : Gestion des actions des boutons
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
#include "process_button_actions.h"
#include "interface_buttons.h"
#include "service_scheduler.h"
#include "driver_HUB75E.h"
#include "process_clock.h"
#include "data_config.h"

/* Variables -----------------------------------------------------------------*/
PROCESS_BUTTON_ACTIONS process_button_actions;

/* Fonctions -----------------------------------------------------------------*/

//=============================================================================
void process_button_actions_init(void)
//
// Fonction d'initialisation des valeurs de la struct processusBouton
//=============================================================================
{
  scheduler_phase_array[PHASE_PROCESS_BUTTONS] = process_button_actions_run;
  
  process_button_actions.button1_press_callback = do_nothing;
  process_button_actions.button2_press_callback = do_nothing;
  process_button_actions.button3_press_callback = do_nothing;

  process_button_actions.button1_release_callback = do_nothing;
  process_button_actions.button2_release_callback = do_nothing;
  process_button_actions.button3_release_callback = do_nothing;
}

//=============================================================================
void process_button_actions_run(void)
//
//=============================================================================
{
  if (interface_button1.information == INFORMATION_AVAILABLE)
  {
    interface_button1.information = INFORMATION_HANDLED;
    if (interface_button1.button_state == INTERFACEBUTTON_HELD)
    {
      process_button_actions.button1_press_callback();
    }
    else
    {
      process_button_actions.button1_release_callback();
    }
  }

  if (interface_button2.information == INFORMATION_AVAILABLE)
  {
    interface_button2.information = INFORMATION_HANDLED;
    if (interface_button2.button_state == INTERFACEBUTTON_HELD)
    {
      process_button_actions.button2_press_callback();
    }
    else
    {
      process_button_actions.button2_release_callback();
    }
  }

  if (interface_button3.information == INFORMATION_AVAILABLE)
  {
    interface_button3.information = INFORMATION_HANDLED;
    if (interface_button3.button_state == INTERFACEBUTTON_HELD)
    {
      process_clock.awake_until_epoch = process_clock.epoch +(data_config_user_setup.desired_wake_minutes * 60);
    }
    else
    {
      process_button_actions.button3_release_callback();
    }
  }
}