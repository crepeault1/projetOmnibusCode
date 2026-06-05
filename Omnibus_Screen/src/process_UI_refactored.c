/**
 ******************************************************************************
 * @file           : process_UI.c
 * @brief          : Dispatcher-based UI runtime.
 * @author         : Samuel Crepeault
 *
 ******************************************************************************
 * @details
 *
 * Architecture:
 *
 *   Input chain (unchanged)  ─►  on_buttonN_press / on_encoder_*
 *                                       │
 *                                       │ sets bits / accumulates delta
 *                                       ▼
 *                            state.button_events / state.encoder_delta
 *                                       │
 *                                       │ drained each tick by process_UI_run
 *                                       ▼
 *                              ui_dispatch_select / _menu / _encoder
 *                                       │
 *                                       │ switch on state.current->behavior
 *                                       ▼
 *                              per-behavior default handler
 *                                       │
 *                                       │ may set state.pending
 *                                       ▼
 *                              process_UI_run honors transition:
 *                                  on_exit (per-behavior + per-screen)
 *                                  swap current
 *                                  reset transient state
 *                                  on_arrival (per-behavior + per-screen)
 *
 * Callbacks are wired ONCE in process_UI_init and never rebound at runtime.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "process_UI_refactored.h"

#include "process_button_actions.h"
#include "process_rotary_encoder_actions.h"
#include "process_communication.h"
#include "frame_assembly.h"
#include "service_scheduler.h"
#include "driver_HUB75E.h"
#include "data_config.h"

#include <string.h>

/* Defines -------------------------------------------------------------------*/
#define CURSOR_DEFAULT_POSITION 3
#define SCROLL_VISIBLE 3

#define EXIT_CLEAR true  // on_exit said: proceed with the transition
#define EXIT_BLOCK false // on_exit said: cancel the transition

#define INITIAL_SCREEN_INDEX 20 // main_screen

/* Variables -----------------------------------------------------------------*/
PROCESS_UI_MASTER_STATE state;
/* Static prototypes ---------------------------------------------------------*/

// Input stubs (wired once, never change)
static void on_button1_press(void);
static void on_button2_press(void);
static void on_encoder_button_press(void);
static void on_encoder_cw(void);
static void on_encoder_ccw(void);

// Event dispatchers
static void ui_dispatch_select(PROCESS_UI_MASTER_STATE *st);
static void ui_dispatch_menu(PROCESS_UI_MASTER_STATE *st);
static void ui_dispatch_encoder(PROCESS_UI_MASTER_STATE *st, int8_t delta);

// Lifecycle dispatchers (per-behavior default + per-screen hook)
static void ui_dispatch_on_arrival(PROCESS_UI_MASTER_STATE *st);
static bool ui_dispatch_on_exit(PROCESS_UI_MASTER_STATE *st);

// Navigate
static void ui_def_nav_select(PROCESS_UI_MASTER_STATE *st);
static void ui_def_nav_menu(PROCESS_UI_MASTER_STATE *st);
// no scroll

// List
static void ui_def_list_select(PROCESS_UI_MASTER_STATE *st);
// list_menu = nav_menu
static void ui_def_list_encoder(PROCESS_UI_MASTER_STATE *st, int8_t delta);

// Numbers
static void ui_def_num_select(PROCESS_UI_MASTER_STATE *st);
static void ui_def_num_menu(PROCESS_UI_MASTER_STATE *st);
static void ui_def_num_encoder(PROCESS_UI_MASTER_STATE *st, int8_t delta);

// Scroll
// scroll_select = nav_select
// scroll_menu = nav_menu
static void ui_def_scroll_encoder(PROCESS_UI_MASTER_STATE *st, int8_t delta);

// Toggle
static void ui_def_toggle_select(PROCESS_UI_MASTER_STATE *st);
static void ui_def_toggle_encoder(PROCESS_UI_MASTER_STATE *st, int8_t delta);

// Helpers
void ui_request_transition(PROCESS_UI_MASTER_STATE *st, uint8_t screen_index, e_ui_nav_direction direction);

void digits_to_time(const uint8_t *digits, uint8_t num_digits,
                    uint8_t *out_hours, uint8_t *out_minutes);

/* Functions -----------------------------------------------------------------*/

//==============================================================================
void process_UI_init(void)
//
//==============================================================================
{
    // Zero everything. current == NULL is important: it signals "no screen
    // active yet", so the first transition's on_exit dispatch is skipped.
    memset(&state, 0, sizeof(state));

    process_button_actions.button1_press_callback = on_button1_press;
    process_button_actions.button2_press_callback = on_button2_press;
    process_encoder_actions.encoder_clockwise_callback = on_encoder_cw;
    process_encoder_actions.encoder_counterclockwise_callback = on_encoder_ccw;
    process_encoder_actions.encoder_button_press_callback = on_encoder_button_press;

    scheduler_phase_array[PHASE_PROCESS_UI] = process_UI_run;

    ui_request_transition(&state, INITIAL_SCREEN_INDEX, UI_NAV_FORWARD);
}

static void on_button1_press(void) { state.button_events |= BTN_SELECT; }
static void on_button2_press(void) { state.button_events |= BTN_MENU; }
static void on_encoder_button_press(void) { state.button_events |= BTN_ENCODER; }
static void on_encoder_cw(void) { state.encoder_delta++; }
static void on_encoder_ccw(void) { state.encoder_delta--; }

//==============================================================================
void process_UI_run(void)
//==============================================================================
{
    uint8_t btns = state.button_events;
    int8_t delta = state.encoder_delta;
    state.button_events = 0;
    state.encoder_delta = 0;

    if (state.current != NULL)
    {
        if (btns & BTN_SELECT)
            ui_dispatch_select(&state);
        if (btns & BTN_MENU)
            ui_dispatch_menu(&state);
        if (delta != 0)
            ui_dispatch_encoder(&state, delta);
    }

    if (state.current != NULL && state.current->screen_on_tick != NULL)
    {
        state.current->screen_on_tick(&state);
    }

    if (state.pending != NULL)
    {
        if (ui_dispatch_on_exit(&state) == EXIT_BLOCK)
        {
            state.pending = NULL;
            return;
        }

        state.current = state.pending;
        state.pending = NULL;

        state.cursor_position = 0;
        state.digit_counter = 0;
        state.scroll_offset = 0;
        memset(state.selected_digit, 0, sizeof(state.selected_digit));

        uint8_t zero_accent[4] = {0, 0, 0, 0};
        character_array_copy(state.current->screen_text, zero_accent);

        ui_dispatch_on_arrival(&state);
        state.render_dirty = true;
    }
}

//==============================================================================
void ui_request_transition(PROCESS_UI_MASTER_STATE *st, uint8_t screen_index, e_ui_nav_direction direction)
//
//==============================================================================
{
    st->pending = data_UI_get_screen_data(screen_index);
    st->pending_direction = direction;
}

//==============================================================================
static void ui_dispatch_select(PROCESS_UI_MASTER_STATE *st)
// Event dispatchers — one switch per event type. Adding a new behavior is
// one case per dispatcher; adding a new screen of an existing behavior is
// pure data, no code changes here.
//==============================================================================
{
    switch (st->current->behavior)
    {
    case UI_BEHAVIOR_WAIT: /* ignore inputs */
        break;
    case UI_BEHAVIOR_NAVIGATE:
        ui_def_nav_select(st);
        break;
    case UI_BEHAVIOR_MENU_LIST:
        ui_def_list_select(st);
        break;
    case UI_BEHAVIOR_SCROLL:
        ui_def_nav_select(st);
        break;
    case UI_BEHAVIOR_NUMERIC_ENTRY:
        ui_def_num_select(st);
        break;
    case UI_BEHAVIOR_TEXT_ENTRY: /* to implement */
        break;
    case UI_BEHAVIOR_TOGGLE:
        ui_def_toggle_select(st);
        break;
    case UI_BEHAVIOR_CUSTOM:
        ui_def_nav_select(st);
        break;
    }
}

static void ui_dispatch_menu(PROCESS_UI_MASTER_STATE *st)
{
    switch (st->current->behavior)
    {
    case UI_BEHAVIOR_WAIT:
        break;
    case UI_BEHAVIOR_NAVIGATE:
    case UI_BEHAVIOR_MENU_LIST:
    case UI_BEHAVIOR_SCROLL:
    case UI_BEHAVIOR_CUSTOM:
    case UI_BEHAVIOR_TOGGLE:
        ui_def_nav_menu(st);
        break;
    case UI_BEHAVIOR_NUMERIC_ENTRY:
        ui_def_num_menu(st);
        break;
    case UI_BEHAVIOR_TEXT_ENTRY: /* to implement */
        break;
    }
}

static void ui_dispatch_encoder(PROCESS_UI_MASTER_STATE *st, int8_t delta)
{
    switch (st->current->behavior)
    {
    case UI_BEHAVIOR_MENU_LIST:
        ui_def_list_encoder(st, delta);
        break;
    case UI_BEHAVIOR_NUMERIC_ENTRY:
        ui_def_num_encoder(st, delta);
        break;
    case UI_BEHAVIOR_SCROLL:
        ui_def_scroll_encoder(st, delta);
        break;
    case UI_BEHAVIOR_TEXT_ENTRY: /* to implement */
        break;
    case UI_BEHAVIOR_TOGGLE:
        ui_def_toggle_encoder(st, delta);
        break;
    default: /* ignore */
        break;
    }
}

//==============================================================================
static void ui_dispatch_on_arrival(PROCESS_UI_MASTER_STATE *st)
//
//==============================================================================
{
    switch (st->current->behavior)
    {
    case UI_BEHAVIOR_NAVIGATE:
        load_accent_region(0);
        break;
    case UI_BEHAVIOR_MENU_LIST:
        // Park cursor at the top of the list.
        draw_cursor(CURSOR_DEFAULT_POSITION);
        break;

    case UI_BEHAVIOR_NUMERIC_ENTRY:
        switch (st->current->index)
        {
        case 44:
        case 47:
            for (uint8_t i = 0; i < st->current->num_digits && i < MAX_DIGITS; i++)
            {
                write_character_to_array('0', 5, 2 + 3 * i);
            }
            break;
        case 61:
        case 62:
        case 63:
        case 64:
        //4 digits but only two draw spaces
            for (uint8_t i = 0; i < (st->current->num_digits -2) && i < MAX_DIGITS; i++)
            {
                write_character_to_array('0', 5, 1 + 5 * i);
                write_character_to_array('0', 5, 2 + 5 * i);
            }
            break;
        }
        load_accent_region(0);
        break;
    case UI_BEHAVIOR_SCROLL:
        break;
    case UI_BEHAVIOR_TOGGLE:
        draw_cursor(CURSOR_DEFAULT_POSITION);
    default:
        break;
    }

    if (st->current->screen_on_arrival != NULL)
    {
        st->current->screen_on_arrival(st);
    }
}

//==============================================================================
static bool ui_dispatch_on_exit(PROCESS_UI_MASTER_STATE *st)
//
//==============================================================================
{
    if (st->current == NULL)
    {
        return EXIT_CLEAR; // No active screen yet (first transition).
    }

    if (st->pending_direction == UI_NAV_FORWARD &&
        st->current->screen_on_exit != NULL)
    {
        if (!st->current->screen_on_exit(st))
        {
            return EXIT_BLOCK;
        }
    }

    // Per-behavior cleanup.
    switch (st->current->behavior)
    {
    case UI_BEHAVIOR_MENU_LIST:
        // Wipe the cursor glyph before the next screen renders over it.
        restore_under_cursor(CURSOR_DEFAULT_POSITION + st->cursor_position);
        break;

    case UI_BEHAVIOR_NUMERIC_ENTRY:
    {
        // Clear the accent region so the next screen starts clean.
        uint8_t zero[4] = {0, 0, 0, 0};
        accent_region_copy(zero);
        break;
    }

    default:
        break;
    }

    return EXIT_CLEAR;
}

//==============================================================================
// Default handlers: NAVIGATE
//==============================================================================
static void ui_def_nav_select(PROCESS_UI_MASTER_STATE *st)
{
    ui_request_transition(st, st->current->next_screen[0], UI_NAV_FORWARD);
}

static void ui_def_nav_menu(PROCESS_UI_MASTER_STATE *st)
{
    ui_request_transition(st, st->current->previous_screen, UI_NAV_BACKWARD);
}

//==============================================================================
// Default handlers: MENU_LIST
//==============================================================================
static void ui_def_list_select(PROCESS_UI_MASTER_STATE *st)
{
    uint8_t idx;
    if (st->current->next_screen_count > 1 && st->current->cursor_count > 1)
    {
        idx = st->cursor_position;
    }
    else
    {
        idx = 0;
    }
    ui_request_transition(st, st->current->next_screen[idx], UI_NAV_FORWARD);
}

static void ui_def_list_encoder(PROCESS_UI_MASTER_STATE *st, int8_t delta)
{
    uint8_t n = st->current->cursor_count;
    if (n <= 1)
        return; // Guard divide-by-zero and pointless moves.

    restore_under_cursor(CURSOR_DEFAULT_POSITION + st->cursor_position);

    // Signed-safe modulo. `delta` can be negative and (after acceleration,
    // if you add it later) larger than n.
    int16_t p = (int16_t)st->cursor_position + delta;
    p = ((p % (int16_t)n) + (int16_t)n) % (int16_t)n;
    st->cursor_position = (uint8_t)p;

    draw_cursor(CURSOR_DEFAULT_POSITION + st->cursor_position);
    st->render_dirty = true;
}

//==============================================================================
// Default handlers: NUMERIC_ENTRY
//==============================================================================
static void ui_def_num_select(PROCESS_UI_MASTER_STATE *st)
{
    if (st->digit_counter < st->current->num_digits)
    {
        // Not yet at Valider — advance focus.
        st->digit_counter++;
        load_accent_region(st->digit_counter);
        st->render_dirty = true;
    }
    else
    {
        // On Valider — commit the entered value and transition.
        // TODO: plug commit_numeric_value(st) into your settings layer.
        ui_request_transition(st, st->current->next_screen[0], UI_NAV_FORWARD);
    }
}

static void ui_def_num_menu(PROCESS_UI_MASTER_STATE *st)
{
    write_string_to_array("              ", 7, 3, 0, 14); // clear invalid message
    if (st->digit_counter > 0)
    {
        // Step back one position. Also zero the digit we're stepping back
        // *into*, matching the original "menu = abandon this digit" feel.
        st->digit_counter--;
        load_accent_region(st->digit_counter);
        st->render_dirty = true;
    }
    else
    {
        ui_request_transition(st, st->current->previous_screen, UI_NAV_BACKWARD);
    }
}

static void ui_def_num_encoder(PROCESS_UI_MASTER_STATE *st, int8_t delta)
{
    if (st->digit_counter >= st->current->num_digits)
    {
        return; // On Valider; encoder does nothing.
    }

    uint8_t base = st->current->digit_max[st->digit_counter];
    if (base == 0)
        base = 10; // 0 means "not set, use default"

    // Signed-safe modulo by 10. Robust to accumulated delta from a fast spin.
    int16_t v = (int16_t)st->selected_digit[st->digit_counter] + delta;
    v = ((v % (int16_t)base) + (int16_t)base) % (int16_t)base;

    st->selected_digit[st->digit_counter] = (uint8_t)v;

    switch (st->current->index)
    {
    case 44:
    case 47:
        write_character_to_array((uint8_t)('0' + v), 5, 2 + 3 * st->digit_counter);
        break;
    case 61:
    case 62:
    case 63:
    case 64:
    case 65:
        if (st->digit_counter < 2)
        {
            write_character_to_array((uint8_t)('0' + v), 5, 1 + 1 * st->digit_counter);
        }
        else
        {
            write_character_to_array((uint8_t)('0' + v), 5, 6 + 1 * (st->digit_counter - 2));
        }
        break;
    }

    st->render_dirty = true;
}

//==============================================================================
// Default handlers: SCROLL
//==============================================================================

static void ui_def_scroll_encoder(PROCESS_UI_MASTER_STATE *st, int8_t delta)
{
    switch (st->current->index)
    {
    case 46:
        st->scroll_limit = data_config_bus_data_dummy.number_of_stops; // bad fix: updates the max scroll every scroll!
        break;
    case 48:
        st->scroll_limit = data_config_bus_data_dummy.number_of_lines; // bad fix: updates the max scroll every scroll!
        break;
    }

    uint8_t loop_value = st->scroll_limit;
    int8_t scroll = (st->scroll_offset + delta);

    if (st->scroll_offset < scroll)
    {
        load_accent_region(1);
    }
    else
    {
        load_accent_region(0);
    }

    scroll = ((scroll % loop_value) + loop_value) % loop_value;

    for (unsigned char i = 0; i < SCROLL_VISIBLE; i++)
    {
        switch (st->current->index) // Screen indexes, 46 is stop number and name, 48 is just line number
        {
        case 46:
            write_number_to_array(data_config_bus_data_dummy.timetable_stop_list[(scroll + i) % loop_value], i + 4, 0, 4);
            write_string_to_array(data_config_bus_data_dummy.timetable_stop_names[(scroll + i) % loop_value], i + 4, 7, 0, 11);
            break;
        case 48:
            write_number_to_array(data_config_bus_data_dummy.line_list[(scroll + i) % loop_value], i + 4, 9, 3);
            break;
        case 71:
            break;
        default:
            break;
        }
    }
    st->scroll_offset = scroll;
}

//==============================================================================
// Default handlers: TOGGLE
//==============================================================================
static void ui_def_toggle_select(PROCESS_UI_MASTER_STATE *st)
{
    switch (st->current->index)
    {
    case 83:
        data_config_user_setup.desired_time_format = st->cursor_position; // 0 = 24h, 1 = 12h
        load_accent_region(data_config_user_setup.desired_time_format);   // highlight whichever option is now selected
        ui_request_transition(st, st->current->next_screen[0], UI_NAV_FORWARD);
        break;
    case 60:
        data_config_user_setup.desired_display_mode = st->cursor_position; // 0 = 24h, 1 = 12h
        ui_request_transition(st, st->current->next_screen[st->cursor_position], UI_NAV_FORWARD);
        break;
    }
}

static void ui_def_toggle_encoder(PROCESS_UI_MASTER_STATE *st, int8_t delta)
{
    uint8_t n = st->current->cursor_count;

    restore_under_cursor(CURSOR_DEFAULT_POSITION + st->cursor_position);
    int16_t p = (int16_t)st->cursor_position + delta;
    p = ((p % (int16_t)n) + (int16_t)n) % (int16_t)n;
    st->cursor_position = (uint8_t)p;
    draw_cursor(CURSOR_DEFAULT_POSITION + st->cursor_position);

    st->render_dirty = true;
}

//==============================================================================
// load_accent_region: read the indexed (start, end) rectangle from the
// current screen and publish it into local_frame[8] via accent_region_copy().
// Exposed in process_UI.h so per-screen hooks can use it.
//==============================================================================
void load_accent_region(uint8_t region)
{
    uint8_t r[4];
    r[0] = state.current->accent_regions[region][0][0];
    r[1] = state.current->accent_regions[region][0][1];
    r[2] = state.current->accent_regions[region][1][0];
    r[3] = state.current->accent_regions[region][1][1];
    accent_region_copy(r);
}

//==============================================================================
// ARRIVAL / EXITS
//==============================================================================

bool arrival_20(PROCESS_UI_MASTER_STATE *st)
{
    display_stops();
}

bool tick_20(PROCESS_UI_MASTER_STATE *st)
{
    display_stops();
}

bool arrival_40(PROCESS_UI_MASTER_STATE *st)
{
    // Nothing to remove
    if (data_config_user_setup.number_added_stops == 0)
    {
        active_stops_screen.next_screen[1] = 42;
    }
    else
    {
        active_stops_screen.next_screen[1] = 51;
    }

    // Too full
    if (data_config_user_setup.number_added_stops >= MAX_STOPS)
    {
        active_stops_screen.next_screen[0] = 41;
    }
    else
    {
        active_stops_screen.next_screen[0] = 43;
    }
}

bool arrival_41(PROCESS_UI_MASTER_STATE *st)
{
    load_accent_region(0);
}

bool arrival_42(PROCESS_UI_MASTER_STATE *st)
{
    load_accent_region(0);
}

bool exit_43(PROCESS_UI_MASTER_STATE *st)
{
    data_config_bus_data_dummy.line_counter = 0;
    data_config_bus_data_dummy.direction_counter = 0;
    memset(data_config_bus_data_dummy.line_list, 0, 300);
    memset(data_config_bus_data_dummy.directions[0], 0, 18);
    memset(data_config_bus_data_dummy.directions[1], 0, 18);
}

bool exit_44(PROCESS_UI_MASTER_STATE *st)
{
    data_config_user_setup.current_line_numbers[0] = st->selected_digit[0];
    data_config_user_setup.current_line_numbers[1] = st->selected_digit[1];
    data_config_user_setup.current_line_numbers[2] = st->selected_digit[2];

    data_config_user_setup.selected_line = st->selected_digit[0] * 100 + st->selected_digit[1] * 10 + st->selected_digit[2];

    // Bus data cleanup
    data_config_bus_data_dummy.direction_counter = 0;
    memset(data_config_bus_data_dummy.directions[0], ' ', 18);
    memset(data_config_bus_data_dummy.directions[1], ' ', 18);

    process_communication_outbound_code = CHECK_LINE;
    return process_communication_exit_block;
}

// ------------------------------------ 45 ------------------------------------

bool arrival_45(PROCESS_UI_MASTER_STATE *st)
{
    process_communication_exit_block = EXIT_BLOCK; // reset for 44 in case of backtrack
    write_character_to_array(data_config_user_setup.current_line_numbers[0] + 0x30, 2, 6);
    write_character_to_array(data_config_user_setup.current_line_numbers[1] + 0x30, 2, 7);
    write_character_to_array(data_config_user_setup.current_line_numbers[2] + 0x30, 2, 8);

    write_string_to_array(data_config_bus_data_dummy.directions[0], 3, 0, 0, 18);
    write_string_to_array(data_config_bus_data_dummy.directions[1], 4, 0, 0, 18);
    return EXIT_CLEAR;
}

bool exit_45(PROCESS_UI_MASTER_STATE *st)
{
    data_config_user_setup.selected_direction = st->cursor_position;

    // Cleanup stop array before getting stops
    memset(
        data_config_bus_data_dummy.timetable_stop_list, 0,
        sizeof(data_config_bus_data_dummy.timetable_stop_list));
    memset(
        data_config_bus_data_dummy.timetable_stop_names, 0,
        sizeof(data_config_bus_data_dummy.timetable_stop_names));
    data_config_bus_data_dummy.stop_counter = 0;

    process_communication_outbound_code = GET_STOPS;
    return EXIT_CLEAR;
}

// ------------------------------------ 46 ------------------------------------

bool arrival_46(PROCESS_UI_MASTER_STATE *st)
{
    process_communication_exit_block = EXIT_BLOCK;
    write_character_to_array(data_config_user_setup.current_line_numbers[0] + 0x30, 2, 14);
    write_character_to_array(data_config_user_setup.current_line_numbers[1] + 0x30, 2, 15);
    write_character_to_array(data_config_user_setup.current_line_numbers[2] + 0x30, 2, 16);
    for (unsigned char i = 0; i < SCROLL_VISIBLE; i++)
    {
        write_number_to_array(data_config_bus_data_dummy.timetable_stop_list[i], i + 4, 0, 4);
        write_string_to_array(data_config_bus_data_dummy.timetable_stop_names[i], i + 4, 7, 0, 11);
    }

    return EXIT_CLEAR;
}

bool tick_46(PROCESS_UI_MASTER_STATE *st)
{
}

bool exit_46(PROCESS_UI_MASTER_STATE *st)
{
    data_config_user_setup.selected_stop = data_config_bus_data_dummy.timetable_stop_list[(st->scroll_offset + 1) % data_config_bus_data_dummy.number_of_stops];
    return EXIT_CLEAR;
}

// ------------------------------------ 47 ------------------------------------
bool arrival_47(PROCESS_UI_MASTER_STATE *st)
{
}

bool exit_47(PROCESS_UI_MASTER_STATE *st)
{
    uint16_t stop_ID;
    data_config_user_setup.current_stop_numbers[0] = st->selected_digit[0];
    data_config_user_setup.current_stop_numbers[1] = st->selected_digit[1];
    data_config_user_setup.current_stop_numbers[2] = st->selected_digit[2];
    data_config_user_setup.current_stop_numbers[3] = st->selected_digit[3];

    stop_ID = st->selected_digit[0] * 1000 +
              st->selected_digit[1] * 100 +
              st->selected_digit[2] * 10 +
              st->selected_digit[3];

    data_config_user_setup.selected_stop = stop_ID;
    st->entered_stop_id = stop_ID;

    // bus data cleanup
    data_config_bus_data_dummy.line_counter = 0;
    memset(
        data_config_bus_data_dummy.line_list, 0,
        sizeof(data_config_bus_data_dummy.line_list));

    process_communication_outbound_code = CHECK_STOP;
    return process_communication_exit_block;
}

// ------------------------------------ 48 ------------------------------------

bool arrival_48(PROCESS_UI_MASTER_STATE *st)
{
    process_communication_exit_block = EXIT_BLOCK; // reset for 44 in case of backtrack
    // process_communication_outbound_code = GET_LINES;
    write_number_to_array(data_config_user_setup.selected_stop, 2, 14, 4);
    for (unsigned char i = 0; i < 3; i++)
    {
        write_number_to_array(data_config_bus_data_dummy.line_list[i], i + 4, 9, 3);
    }
    return EXIT_CLEAR;
}

bool exit_48(PROCESS_UI_MASTER_STATE *st)
{
    data_config_user_setup.selected_line = data_config_bus_data_dummy.line_list[(st->scroll_offset + 1) % data_config_bus_data_dummy.number_of_lines];
    return EXIT_CLEAR;
}

bool arrival_49(PROCESS_UI_MASTER_STATE *st)
{
    // Stop
    data_config_user_setup.added_stops_and_lines[data_config_user_setup.added_stops_and_lines_index][0] = data_config_user_setup.selected_stop;
    // Line
    data_config_user_setup.added_stops_and_lines[data_config_user_setup.added_stops_and_lines_index][1] = data_config_user_setup.selected_line;

    data_config_user_setup.added_stops_and_lines_index++;
    data_config_user_setup.number_added_stops++;
    process_communication_outbound_code = WAIT_TIMES;
    return EXIT_CLEAR;
}

bool exit_51(PROCESS_UI_MASTER_STATE *st)
{
    data_config_user_setup.added_stops_and_lines_index = 0;
    data_config_user_setup.number_added_stops = 0;
    for (int i = 0; i < MAX_STOPS; i++)
    {
        data_config_user_setup.added_stops_and_lines[i][0] = 0;
        data_config_user_setup.added_stops_and_lines[i][1] = 0;
    }
    process_communication_outbound_code = RESET;
}

bool arrival_60(PROCESS_UI_MASTER_STATE *st)
{
    load_accent_region(data_config_user_setup.desired_display_mode);
}

bool exit_61(PROCESS_UI_MASTER_STATE *st)
{
    uint8_t h, m;
    digits_to_time(st->selected_digit, st->current->num_digits, &h, &m);
    data_config_user_setup.desired_start_hour   = h;
    data_config_user_setup.desired_start_minute = m;
    return EXIT_CLEAR;
}

bool exit_62(PROCESS_UI_MASTER_STATE *st)
{
    uint8_t h, m;
    digits_to_time(st->selected_digit, st->current->num_digits, &h, &m);
    data_config_user_setup.desired_stop_hour   = h;
    data_config_user_setup.desired_stop_minute = m;
    return EXIT_CLEAR;
}

bool exit_65(PROCESS_UI_MASTER_STATE *st)
{
    uint16_t minutes;
    minutes = 10 * st->selected_digit[0] + st->selected_digit[1];
    data_config_user_setup.desired_wake_minutes = minutes;
}

bool arrival_83(PROCESS_UI_MASTER_STATE *st)
{
    load_accent_region(data_config_user_setup.desired_time_format);
    write_string_to_array("  ", 0, 9, 0, 2);
    return EXIT_CLEAR;
}

void digits_to_time(const uint8_t *digits, uint8_t num_digits,
                    uint8_t *out_hours, uint8_t *out_minutes)
{
    uint8_t h_count = num_digits / 2;

    uint8_t h = 0;
    for (uint8_t i = 0; i < h_count; i++) {
        h = h * 10 + digits[i];
    }

    uint8_t m = 0;
    for (uint8_t i = h_count; i < num_digits; i++) {
        m = m * 10 + digits[i];
    }

    *out_hours   = h;
    *out_minutes = m;
}