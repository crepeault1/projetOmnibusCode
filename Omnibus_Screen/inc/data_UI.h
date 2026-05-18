#ifndef DATAUI_H
#define DATAUI_H
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
typedef enum {
    UI_BEHAVIOR_WAIT,
    UI_BEHAVIOR_NAVIGATE,        // Select=next, Menu=prev
    UI_BEHAVIOR_MENU_LIST,       // Select moves highlight, Select-on-last picks
    UI_BEHAVIOR_SCROLL,          // Cursor is static, encoder moves text
    UI_BEHAVIOR_NUMERIC_ENTRY,   // Encoder edits digit, Select advances digit
    UI_BEHAVIOR_TEXT_ENTRY,      // Encoder scrolls text, Select chooses character
    UI_BEHAVIOR_TOGGLE,          // Select flips a bool
    UI_BEHAVIOR_CUSTOM           // Use override function pointers
} ui_behavior_t;

typedef struct
{
  //Moving between screens
  uint8_t index;
  uint8_t next_screen[5];
  uint8_t next_screen_count;
  uint8_t cursor_count;
  uint8_t previous_screen;
  
  //What a screen can do
  ui_behavior_t behavior;
  uint8_t select_button_action;
  uint8_t max_select_actions;
  uint8_t num_digits;
  uint8_t digit_positions[4][2];
  uint8_t menu_button_actions;

  //What a screen holds
  const uint8_t screen_text[8][22]; // Display text
} PROCESS_UI_SCREEN;

extern PROCESS_UI_SCREEN error_screen; // 0

extern PROCESS_UI_SCREEN boot_screen;    // 10
extern PROCESS_UI_SCREEN tooltip_screen; // 11

extern PROCESS_UI_SCREEN main_screen; // 20

extern PROCESS_UI_SCREEN menu_screen; // 30

extern PROCESS_UI_SCREEN active_stops_screen;   // 40
extern PROCESS_UI_SCREEN too_many_stops_screen; // 41
extern PROCESS_UI_SCREEN no_stops_screen;       // 42
extern PROCESS_UI_SCREEN add_stop_screen;       // 43
extern PROCESS_UI_SCREEN by_line_1_screen;      // 44
extern PROCESS_UI_SCREEN by_line_2_screen;      // 45
extern PROCESS_UI_SCREEN by_line_3_screen;      // 46
extern PROCESS_UI_SCREEN by_stop_1_screen;      // 47
extern PROCESS_UI_SCREEN by_stop_2_screen;      // 48
extern PROCESS_UI_SCREEN stop_added_screen;     // 49
extern PROCESS_UI_SCREEN remove_stop_screen;    // 50
extern PROCESS_UI_SCREEN stop_removed_screen;   // 51

extern PROCESS_UI_SCREEN display_mode_screen;       // 60
extern PROCESS_UI_SCREEN time_activate_24_1_screen; // 61
extern PROCESS_UI_SCREEN time_activate_24_2_screen; // 62
extern PROCESS_UI_SCREEN time_activate_12_1_screen; // 63
extern PROCESS_UI_SCREEN time_activate_12_2_screen; // 64
extern PROCESS_UI_SCREEN wake_duration_screen;      // 65
extern PROCESS_UI_SCREEN changes_saved_screen;      // 66

extern PROCESS_UI_SCREEN wifi_connection_screen;        // 70
extern PROCESS_UI_SCREEN wifi_manual_1_screen;          // 71
extern PROCESS_UI_SCREEN wifi_manual_2_screen;          // 72
extern PROCESS_UI_SCREEN wifi_automatic_1_screen;       // 73
extern PROCESS_UI_SCREEN wifi_trying_connection_screen; // 74
extern PROCESS_UI_SCREEN wifi_success_screen;           // 75
extern PROCESS_UI_SCREEN wifi_failure_screen;           // 76

extern PROCESS_UI_SCREEN data_sync_1_screen; // 80
extern PROCESS_UI_SCREEN data_sync_2_screen; // 81
extern PROCESS_UI_SCREEN data_sync_3_screen; // 82
extern PROCESS_UI_SCREEN time_format_screen; // 83

/* Prototypes ----------------------------------------------------------------*/
void process_UI_init(void);
PROCESS_UI_SCREEN *data_UI_get_screen_data(uint8_t screen_index);

#endif