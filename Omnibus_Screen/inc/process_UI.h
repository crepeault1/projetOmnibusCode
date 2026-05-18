#ifndef PROCESSUI_H
#define PROCESSUI_H
/**
  ******************************************************************************
  * @file           : process_UI.h
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

/* Defines -------------------------------------------------------------------*/

/* Variables ---------------------------------------------------------------- */
typedef struct
{
  unsigned char ready_for_rendering;
  unsigned char button_state;
  unsigned int debounce_counter;
  unsigned int read_counter;
} PROCESS_UI;

/* Prototypes ----------------------------------------------------------------*/
void process_UI_init(void);
void process_UI_run(void);
void process_UI_render_next(void);
void process_UI_render_previous(void);

void process_UI_cursor_up(void);
void process_UI_cursor_down(void);
void process_UI_scroll_up(void);
void process_UI_scroll_down(void);

void process_UI_next_digit(void);
void process_UI_previous_digit(void);
void process_UI_digit_up(void);
void process_UI_digit_down(void);

//DEBUG
void process_UI_button_tester1(void);
void process_UI_button_tester2(void);
#endif