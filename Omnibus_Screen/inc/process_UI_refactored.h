#ifndef PROCESSUI1_H
#define PROCESSUI1_H
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
#include "data_UI_refactored.h"

/* Defines -------------------------------------------------------------------*/
#define BTN_SELECT   (1u << 0)
#define BTN_MENU     (1u << 1)
#define BTN_ENCODER  (1u << 2)

/* Variables ---------------------------------------------------------------- */
typedef enum {
    UI_NAV_FORWARD,    // user committed: run screen_on_exit hook
    UI_NAV_BACKWARD,   // user cancelled: skip screen_on_exit hook
    UI_NAV_AUTO,       // programmatic (timeout, async result): treat as forward
} e_ui_nav_direction;


// Full definition of the type forward-declared in data_UI.h.
// One global instance lives in process_UI.c.
struct PROCESS_UI_MASTER_STATE_ {
    const PROCESS_UI_SCREEN *current;
    const PROCESS_UI_SCREEN *pending;     // requested transition, NULL if none
    e_ui_nav_direction       pending_direction;

    // Cursor / list selection
    uint8_t  cursor_position;

    // Numeric / text entry
    uint8_t  digit_counter;
    uint8_t  selected_digit[MAX_DIGITS];

    // Scroll
    int8_t   scroll_offset;
    uint8_t scroll_limit;

    // Input — set by ISR/poll callbacks, consumed by run loop
    volatile int8_t   encoder_delta;   // accumulated, signed
    volatile uint8_t  button_events;   // bitfield: BTN_SELECT, BTN_MENU, BTN_ENCODER

    bool render_dirty;

    /* Set by stop-entry's on_exit, read by stop_validating's on_arrival. */
    uint16_t entered_stop_id;

    /* Set by stop_validating on failure, read by stop-entry's on_arrival to
     * display a localized error message. CSV_ERR_NONE means "no error". */
    uint8_t  last_validation_error;
};

extern PROCESS_UI_MASTER_STATE state;

/* Prototypes ----------------------------------------------------------------*/
void process_UI_init(void);
void process_UI_run(void);

void ui_request_transition(PROCESS_UI_MASTER_STATE *st,
                           uint8_t screen_index,
                           e_ui_nav_direction direction);

// Exposed for use inside per-screen on_arrival / on_exit hooks
// (e.g. a screen that wants to highlight a specific accent region during setup).
void load_accent_region(uint8_t region);

//entry-exit
bool arrival_20(PROCESS_UI_MASTER_STATE *st);

bool exit_43(PROCESS_UI_MASTER_STATE *st);

bool exit_44(PROCESS_UI_MASTER_STATE *st);

bool arrival_45(PROCESS_UI_MASTER_STATE *st);
bool exit_45(PROCESS_UI_MASTER_STATE *st);

bool arrival_46(PROCESS_UI_MASTER_STATE *st);
bool exit_46(PROCESS_UI_MASTER_STATE *st);

bool arrival_47(PROCESS_UI_MASTER_STATE *st);
bool exit_47(PROCESS_UI_MASTER_STATE *st);

bool arrival_48(PROCESS_UI_MASTER_STATE *st);
bool exit_48(PROCESS_UI_MASTER_STATE *st);

bool arrival_49(PROCESS_UI_MASTER_STATE *st);

#endif
