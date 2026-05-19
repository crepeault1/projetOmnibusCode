/**
 ******************************************************************************
 * @file           : process_UI.c
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
#include "process_UI.h"
#include "data_UI.h"
#include "process_button_actions.h"
#include "process_rotary_encoder_actions.h"
#include "frame_assembly.h"
#include "service_scheduler.h"
#include "driver_HUB75E.h"
#include "data_config.h"

/* Defines -------------------------------------------------------------------*/
#define CURSOR_DEFAULT_POSITION 3
#define DIGIT_DEFAULT_ROW 6
#define DIGIT_DEFAULT_COLUMN 1

/* Variables -----------------------------------------------------------------*/
PROCESS_UI_SCREEN *current_screen_data;
PROCESS_UI_SCREEN *next_screen_data;
PROCESS_UI_SCREEN *previous_screen_data;
uint8_t current_index;
uint8_t next_screen_index;
int8_t progress_counter;
bool update_necessary;
bool waking_up_from_boot;
bool cursor_is_default;
uint8_t cursor_position;

uint8_t selected_digit[4];
uint8_t digit_counter;
uint8_t selected_accent_region[4];

/* Functions -----------------------------------------------------------------*/

//==============================================================================
void process_UI_init(void)
//
//==============================================================================
{
    // character_buffer_to_pixel_buffer(main_screen.screen_text); //DEBUG
    current_screen_data = data_UI_get_screen_data(20);
    current_index = current_screen_data->index;
    progress_counter = 0;
    cursor_position = 0;
    digit_counter = 0;

    waking_up_from_boot = true;
    update_necessary = true;
    cursor_is_default = true;

    scheduler_phase_array[PHASE_PROCESS_UI] = process_UI_run;
}

//==============================================================================
void process_UI_run(void)
// //TODO: check claude data_ui bugs
//==============================================================================
{
    if (update_necessary)
    {
        update_necessary = false;
        switch (current_screen_data->behavior)
        {
        case UI_BEHAVIOR_WAIT:
            // Nothing to do
            process_button_actions.button1_press_callback = do_nothing;
            process_button_actions.button2_press_callback = do_nothing;
            process_encoder_actions.encoder_clockwise_callback = do_nothing;
            process_encoder_actions.encoder_counterclockwise_callback = do_nothing;
            process_encoder_actions.encoder_button_press_callback = do_nothing;
            break;

        case UI_BEHAVIOR_NAVIGATE:
            // Only one button press before re-rendering
            process_button_actions.button1_press_callback = process_UI_render_next;
            process_button_actions.button2_press_callback = process_UI_render_previous;

            // Encoder does nothing here
            process_encoder_actions.encoder_clockwise_callback = do_nothing;
            process_encoder_actions.encoder_counterclockwise_callback = do_nothing;
            process_encoder_actions.encoder_button_press_callback = do_nothing;
            break;

        case UI_BEHAVIOR_MENU_LIST:
            // Only one button press before re-rendering
            process_button_actions.button1_press_callback = process_UI_render_next;
            process_button_actions.button2_press_callback = process_UI_render_previous;

            // Encoder changes the cursor position
            draw_cursor(CURSOR_DEFAULT_POSITION);
            cursor_is_default = true;
            // process_UI_cursor_down();
            process_encoder_actions.encoder_clockwise_callback = process_UI_cursor_down;
            process_encoder_actions.encoder_counterclockwise_callback = process_UI_cursor_up;
            process_encoder_actions.encoder_button_press_callback = do_nothing;
            break;

        case UI_BEHAVIOR_SCROLL:
            // Only one button press before re-rendering
            process_button_actions.button1_press_callback = process_UI_render_next;
            process_button_actions.button2_press_callback = process_UI_render_previous;

            // Encoder changes the text position
            process_encoder_actions.encoder_clockwise_callback = process_UI_scroll_down;
            process_encoder_actions.encoder_counterclockwise_callback = process_UI_scroll_up;
            process_encoder_actions.encoder_button_press_callback = do_nothing;
            break;

        case UI_BEHAVIOR_NUMERIC_ENTRY:

            load_accent_region(digit_counter);
            // Multiple button presses before re-rendering
            process_button_actions.button1_press_callback = process_UI_next_digit;
            process_button_actions.button2_press_callback = process_UI_render_previous;

            // Encoder changes the cursor position
            process_encoder_actions.encoder_clockwise_callback = process_UI_digit_up;
            process_encoder_actions.encoder_counterclockwise_callback = process_UI_digit_down;
            process_encoder_actions.encoder_button_press_callback = do_nothing;
            break;

        case UI_BEHAVIOR_TEXT_ENTRY:
            break;

        case UI_BEHAVIOR_TOGGLE:
            break;

        case UI_BEHAVIOR_CUSTOM:
            process_button_actions.button1_press_callback = process_UI_render_next;
            process_button_actions.button2_press_callback = process_UI_render_previous;
            break;
        }
    }
}

//==============================================================================
void process_UI_render_next(void)
//
//==============================================================================
{
    uint8_t next_screen_index;
    if (current_screen_data->next_screen_count > 1 && current_screen_data->cursor_count > 1)
    {
        next_screen_index = cursor_position;
    }
    else
    {
        next_screen_index = 0;
    }

    next_screen_data = data_UI_get_screen_data(current_screen_data->next_screen[next_screen_index]);

    character_array_copy(next_screen_data->screen_text, selected_accent_region);

    current_screen_data = next_screen_data;
    progress_counter = 0;
    cursor_position = 0;
    digit_counter = 0;
    selected_accent_region[0] = 0;
    update_necessary = true;
}

//==============================================================================
void process_UI_render_previous(void)
//
//==============================================================================
{
    previous_screen_data = data_UI_get_screen_data(current_screen_data->previous_screen);

    character_array_copy(previous_screen_data->screen_text, selected_accent_region);

    current_screen_data = previous_screen_data;
    progress_counter = 0;
    cursor_position = 0;
    digit_counter = 0;
    selected_accent_region[0] = 0;
    update_necessary = true;
}

//==============================================================================
void process_UI_cursor_up(void)
//
//==============================================================================
{
    restore_under_cursor(CURSOR_DEFAULT_POSITION + cursor_position);
    cursor_position = (cursor_position + current_screen_data->cursor_count - 1) % current_screen_data->cursor_count;
    draw_cursor(CURSOR_DEFAULT_POSITION + cursor_position);
}

//==============================================================================
void process_UI_cursor_down(void)
//
//==============================================================================
{
    restore_under_cursor(CURSOR_DEFAULT_POSITION + cursor_position);
    cursor_position = (cursor_position + 1) % current_screen_data->cursor_count;
    draw_cursor(CURSOR_DEFAULT_POSITION + cursor_position);
}

//==============================================================================
void process_UI_scroll_up(void)
//
//==============================================================================
{
}

//==============================================================================
void process_UI_scroll_down(void)
//
//==============================================================================
{
}

//==============================================================================
void process_UI_next_digit(void)
//
//==============================================================================
{
    if (digit_counter >= (current_screen_data->num_digits - 1))
    {
        process_button_actions.button1_press_callback = process_UI_render_next;
        process_encoder_actions.encoder_clockwise_callback = do_nothing;
        process_encoder_actions.encoder_counterclockwise_callback = do_nothing;
    }
    else
    {
        process_button_actions.button2_press_callback = process_UI_previous_digit;
    }
    digit_counter++;
    load_accent_region(digit_counter);
}

//==============================================================================
void process_UI_previous_digit(void)
//
//==============================================================================
{
    digit_counter--;
    load_accent_region(digit_counter);

    if (digit_counter <= (current_screen_data->num_digits))
    {
        selected_digit[digit_counter] = 0;
    }

    if (digit_counter <= 0)
    {
        process_button_actions.button2_press_callback = process_UI_render_previous;
    }
    else
    {
        process_button_actions.button1_press_callback = process_UI_next_digit;
        process_encoder_actions.encoder_clockwise_callback = process_UI_digit_up;
        process_encoder_actions.encoder_counterclockwise_callback = process_UI_digit_down;
    }
}

//==============================================================================
void process_UI_digit_up(void)
//
//==============================================================================
{
    selected_digit[digit_counter]++;
    selected_digit[digit_counter] %= 10;
    write_character_to_array((selected_digit[digit_counter] + 0x30), 5, 2 + 3 * digit_counter);
}

//==============================================================================
void process_UI_digit_down(void)
//
//==============================================================================
{
    if (selected_digit[digit_counter] == 0)
    {
        selected_digit[digit_counter] = 9;
    }
    else
    {
        selected_digit[digit_counter]--;
    }
    write_character_to_array((selected_digit[digit_counter] + 0x30), 5, 2 + 3 * digit_counter);
}

//==============================================================================
void load_accent_region(uint8_t region)
//
//==============================================================================
{
    selected_accent_region[0] = current_screen_data->accent_regions[region][0][0];
    selected_accent_region[1] = current_screen_data->accent_regions[region][0][1];
    selected_accent_region[2] = current_screen_data->accent_regions[region][1][0];
    selected_accent_region[3] = current_screen_data->accent_regions[region][1][1];
    accent_region_copy(selected_accent_region);
}