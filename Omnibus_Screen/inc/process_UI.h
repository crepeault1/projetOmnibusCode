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
#include "data_UI.h"
/* Defines -------------------------------------------------------------------*/

/* Variables ---------------------------------------------------------------- */

/* typedef struct {
    const PROCESS_UI_SCREEN *current;
    const PROCESS_UI_SCREEN *pending;     // requested transition, NULL if none
    
    // Cursor / list selection
    uint8_t  cursor_position;
    
    // Numeric / text entry
    uint8_t  digit_counter;
    uint8_t  selected_digit[MAX_DIGITS];
    
    // Scroll
    int8_t   scroll_offset;
    
    // Input — set by ISR/poll, consumed by run loop
    volatile int8_t   encoder_delta;   // accumulated, signed
    volatile uint8_t  button_events;   // bitfield: BTN_SELECT, BTN_MENU
    
    bool render_dirty;
} PROCESS_UI_MASTER_STATE; */

/* Prototypes ----------------------------------------------------------------*/
void process_UI_init(void);

void process_UI_run(void);
void process_UI_reset_on_new_screen(void);
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

void load_accent_region(uint8_t region);

#endif