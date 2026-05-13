/**
  ******************************************************************************
  * @file           : interface_buttons.c
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
#include "driver_buttons.h"
#include "service_scheduler.h"
#include "interface_buttons.h"

/* Defines -------------------------------------------------------------------*/
#define INTERFACEBUTTON_COUNT_BEFORE_READING (SCHEDULER_FREQ_HZ/INTERFACEBUTTON_READ_FREQ)
#define INTERFACEBUTTON_INITIAL_DEBOUNCE_VAL (INTERFACEBUTTON_NECESSARY_BUTTON_READS/2)

/* Variables -----------------------------------------------------------------*/
INTERFACE_BUTTON interface_button1;
INTERFACE_BUTTON interface_button2;
INTERFACE_BUTTON interface_button3;

/* Fonctions -----------------------------------------------------------------*/


//=============================================================================
void interface_button1_init(void)
//
//=============================================================================
{
  interface_button1.information = INFORMATION_HANDLED;
  interface_button1.read_counter = 0;
  interface_button1.debounce_counter = INTERFACEBUTTON_INITIAL_DEBOUNCE_VAL;
  scheduler_phase_array[PHASE_INTERFACE_BUTTON1] = interface_button1_run;
}

//=============================================================================
void interface_button2_init(void)
//
//=============================================================================
{
  interface_button2.information = INFORMATION_HANDLED;
  interface_button2.read_counter = 0;
  interface_button2.debounce_counter = INTERFACEBUTTON_INITIAL_DEBOUNCE_VAL;
  scheduler_phase_array[PHASE_INTERFACE_BUTTON2] = interface_button2_run;
}

//=============================================================================
void interface_button3_init(void)
//
//=============================================================================
{
  interface_button3.information = INFORMATION_HANDLED;
  interface_button3.read_counter = 0;
  interface_button3.debounce_counter = INTERFACEBUTTON_INITIAL_DEBOUNCE_VAL;
  scheduler_phase_array[PHASE_INTERFACE_BUTTON3] = interface_button3_run;
}

//=============================================================================
void interface_button1_run(void)
//
//=============================================================================
{
  interface_button1.read_counter++;
  if (interface_button1.read_counter < INTERFACEBUTTON_COUNT_BEFORE_READING)
  {
    return;
  }
  interface_button1.read_counter = 0;
  if (driverIOB1_read() == INTERFACEBUTTON_VALUE_IF_PRESSED)
  {
    if (interface_button1.debounce_counter == INTERFACEBUTTON_NECESSARY_BUTTON_READS)
    {
      return;
    }
    interface_button1.debounce_counter++;
    if (interface_button1.debounce_counter < INTERFACEBUTTON_NECESSARY_BUTTON_READS)
    {
      return;
    }
    interface_button1.button_state = INTERFACEBUTTON_HELD;
    interface_button1.information = INFORMATION_AVAILABLE;
    return;
  }
  if (interface_button1.debounce_counter == 0)
  {
    return; 
  }
  interface_button1.debounce_counter--;
  if (interface_button1.debounce_counter > 0)
  {
    return;
  }
  interface_button1.button_state = INTERFACEBUTTON_RELEASED;
  interface_button1.information = INFORMATION_AVAILABLE;
}

//=============================================================================
void interface_button2_run(void)
//
//=============================================================================
{
  interface_button2.read_counter++;
  if (interface_button2.read_counter < INTERFACEBUTTON_COUNT_BEFORE_READING)
  {
    return;
  }
  interface_button2.read_counter = 0;
  if (driverIOB2_read() == INTERFACEBUTTON_VALUE_IF_PRESSED)
  {
    if (interface_button2.debounce_counter == INTERFACEBUTTON_NECESSARY_BUTTON_READS)
    {
      return;
    }
    interface_button2.debounce_counter++;
    if (interface_button2.debounce_counter < INTERFACEBUTTON_NECESSARY_BUTTON_READS)
    {
      return;
    }
    interface_button2.button_state = INTERFACEBUTTON_HELD;
    interface_button2.information = INFORMATION_AVAILABLE;
    return;
  }
  if (interface_button2.debounce_counter == 0)
  {
    return; 
  }
  interface_button2.debounce_counter--;
  if (interface_button2.debounce_counter > 0)
  {
    return;
  }
  interface_button2.button_state = INTERFACEBUTTON_RELEASED;
  interface_button2.information = INFORMATION_AVAILABLE;
}

//=============================================================================
void interface_button3_run(void)
//
//=============================================================================
{
  interface_button3.read_counter++;
  if (interface_button3.read_counter < INTERFACEBUTTON_COUNT_BEFORE_READING)
  {
    return;
  }
  interface_button3.read_counter = 0;
  if (driverIOB3_read() == INTERFACEBUTTON_VALUE_IF_PRESSED)
  {
    if (interface_button3.debounce_counter == INTERFACEBUTTON_NECESSARY_BUTTON_READS)
    {
      return;
    }
    interface_button3.debounce_counter++;
    if (interface_button3.debounce_counter < INTERFACEBUTTON_NECESSARY_BUTTON_READS)
    {
      return;
    }
    interface_button3.button_state = INTERFACEBUTTON_HELD;
    interface_button3.information = INFORMATION_AVAILABLE;
    return;
  }
  if (interface_button1.debounce_counter == 0)
  {
    return; 
  }
  interface_button3.debounce_counter--;
  if (interface_button3.debounce_counter > 0)
  {
    return;
  }
  interface_button3.button_state = INTERFACEBUTTON_RELEASED;
  interface_button3.information = INFORMATION_AVAILABLE;
}