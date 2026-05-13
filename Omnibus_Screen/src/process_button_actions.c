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
}

//=============================================================================
void process_button_actions_run(void)
//
//=============================================================================
{
  if (interface_button1.information == INFORMATION_AVAILABLE)
  {
    interface_button1.information = INFORMATION_HANDLED;
    if (interface_button1.button_state == INTERFACEBUTTON_VALUE_IF_PRESSED)
    {
    }
    else
    {
    }
  }
  
  if (interface_button2.information == INFORMATION_AVAILABLE)
  {
    interface_button2.information = INFORMATION_HANDLED;
    if (interface_button2.button_state == INTERFACEBUTTON_VALUE_IF_PRESSED)
    {
    }
    else
    {
    }
  }

  if (interface_button3.information == INFORMATION_AVAILABLE)
  {
    interface_button3.information = INFORMATION_HANDLED;
    if (interface_button3.button_state == INTERFACEBUTTON_VALUE_IF_PRESSED)
    {
    }
    else
    {
    }
  }
}