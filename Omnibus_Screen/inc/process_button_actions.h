#ifndef PROCESSBUTTONACTIONS_H
#define PROCESSBUTTONACTIONS_H

/**
 ******************************************************************************
 * @file           : process_button_actions.h
 * @brief          :
 * @author         : Samuel Crepeault
 *
 ******************************************************************************
 * @details
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/

/* Defines -------------------------------------------------------------------*/

/* Variables ---------------------------------------------------------------- */
typedef struct
{
  uint8_t information;
  uint8_t request;
  uint8_t statut;
  void (*button1_press_callback)(void);
  void (*button1_release_callback)(void);

  void (*button2_press_callback)(void);
  void (*button2_release_callback)(void);

  void (*button3_press_callback)(void);
  void (*button3_release_callback)(void);
} PROCESS_BUTTON_ACTIONS;

extern PROCESS_BUTTON_ACTIONS process_button_actions;

/* Prototypes ----------------------------------------------------------------*/
void process_button_actions_init(void);
void process_button_actions_run(void);

#endif