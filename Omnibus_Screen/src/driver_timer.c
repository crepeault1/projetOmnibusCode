/**
 ******************************************************************************
 * @file           : driver_timer.c
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
#include "driver_timer.h"
#include "Service_Scheduler.h"
#include "hardware/structs/systick.h"
/* Defines -------------------------------------------------------------------*/
#define SYSTICK_RELOAD_1MS 149999
//TODO: remplacer cette valeur par le système dynamique


/* Variables -----------------------------------------------------------------*/
volatile bool systick_flag = false;

/* Functions -----------------------------------------------------------------*/
void isr_systick(void)
{
  systick_flag = true;
}

void driver_timer_init(void)
{
    // Disable SysTick before configuring
    systick_hw->csr = 0;

    // Set reload value
    systick_hw->rvr = SYSTICK_RELOAD_1MS;

    // Clear the current value register
    systick_hw->cvr = 0;

    // Enable SysTick:
    // Bit 0 = ENABLE
    // Bit 1 = TICKINT (enable interrupt)
    // Bit 2 = CLKSOURCE (1 = processor clock, 0 = reference clock)
    systick_hw->csr = 0x07;
}