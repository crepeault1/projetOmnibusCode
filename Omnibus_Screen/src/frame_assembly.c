/**
 ******************************************************************************
 * @file           : frame_assembly.c
 * @brief          :
 * @author         : Samuel Crepeault
 *
 ******************************************************************************
 * @details
 *
 * Character buffer --> pixel buffer --> frame assembly
 *
 * @attention
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "data_font_5x7.h"
#include "frame_assembly.h"
#include "service_scheduler.h"
#include "data_UI_refactored.h"
#include "data_config.h"

#include <stdio.h>
#include <string.h>

/* Defines -------------------------------------------------------------------*/
#define BOX_PIXEL_WIDTH 6
#define BOX_PIXEL_HEIGHT 8
#define CHARACT_ARRAY_ROWS 8
#define CHARACT_ARRAY_COLUMNS 21
#define CURSOR_START_COLUMN (CHARACT_ARRAY_COLUMNS - CURSOR_LENGTH)
#define PIXEL_BUFFER_ROWS 64
#define PIXEL_BUFFER_COLUMNS 128

#define MARGIN 1
#define TOP_ROW_MARGIN 1

#define WHITE 0xFFFF
#define GRAY 0xD6BA
#define ACCENT_COLOR 0xFFFF
#define REGULAR_COLOR 0xC426

/* Variables ---------------------------------------------------------------- */
bool render_flag;

uint8_t local_frame[CHARACT_ARRAY_ROWS + 1]     // for screen information
                   [CHARACT_ARRAY_COLUMNS + 1]; // for null-terminator
uint8_t cursor[CURSOR_LENGTH] = {" <<"};
uint8_t text_under_cursor[CURSOR_LENGTH];

uint8_t padding[4] = {0, 0, 0, 0};

bool pixel_buffer_1[PIXEL_BUFFER_ROWS][PIXEL_BUFFER_COLUMNS];
bool pixel_buffer_2[PIXEL_BUFFER_ROWS][PIXEL_BUFFER_COLUMNS];

uint16_t __attribute__((aligned(4))) frame_buffer_1[8192];
uint16_t __attribute__((aligned(4))) frame_buffer_2[8192];
/* Fonctions -----------------------------------------------------------------*/

void frame_assembly_init(void)
{
    render_flag = true;
    character_array_copy(boot_screen.screen_text, padding); // Temporary
    memset(local_frame, ' ', 21);
    scheduler_phase_array[PHASE_PROCESS_FRAME_ASSEMBLY] = character_buffer_to_pixel_buffer;
}

//==============================================================================
void character_array_copy(const uint8_t character_buf[8][22], uint8_t accent_reg[4])
//
//==============================================================================
{
    memcpy(&local_frame[TOP_ROW_MARGIN], &character_buf[TOP_ROW_MARGIN], sizeof(uint8_t) * 154); //7*22
    memcpy(local_frame[8], accent_reg, sizeof(uint8_t) * 4);
}

//==============================================================================
void accent_region_copy(uint8_t accent_reg[4])
//
//==============================================================================
{
    memcpy(local_frame[8], accent_reg, sizeof(uint8_t) * 4);
}

//==============================================================================
void write_character_to_array(uint8_t character, uint8_t row, uint8_t column)
//
//==============================================================================
{
    local_frame[row][column] = character;
}

//==============================================================================
void write_string_to_array(uint8_t str[], uint8_t row, uint8_t column, uint8_t start, uint8_t length)
//
//==============================================================================
{
    if (column + length > CHARACT_ARRAY_COLUMNS)
    {
        length = CHARACT_ARRAY_COLUMNS - column;
    }
    for (unsigned char i = 0; i < length; i++)
    {
        local_frame[row][column + i] = str[start + i];
    }
}

//==============================================================================
void write_number_to_array(uint16_t number, uint8_t row, uint8_t column, uint8_t length)
//
//==============================================================================
{
    //Todo: remplacer par sprintf
    uint8_t number_ascii[5];
    if (column + length > CHARACT_ARRAY_COLUMNS)
    {
        length = CHARACT_ARRAY_COLUMNS - column;
    }
    number_ascii[0] = '0' + (number / 10000);
    number_ascii[1] = '0' + (number % 10000) / 1000;
    number_ascii[2] = '0' + (number % 1000) / 100;
    number_ascii[3] = '0' + (number % 100) / 10;
    number_ascii[4] = '0' + (number % 10);
    for (unsigned char i = 0; i < length; i++)
    {
        local_frame[row][column + i] = number_ascii[i + (5 - length)];
    }
}

void display_stops(void)
{
    uint8_t stop_ascii[4];
    uint8_t line_ascii[3];
    uint8_t display_time_1[5];
    uint8_t display_time_2[5];
    uint8_t display_time_3[3];

    for (unsigned char i = 0; i < data_config_user_setup.number_added_stops; i++)
    {
        uint16_t stop = data_config_user_setup.added_stops_and_lines[i][0];
        uint16_t line = data_config_user_setup.added_stops_and_lines[i][1];
        uint16_t time_1 = data_config_bus_data_dummy.minutes_until_passage[i][0];
        uint16_t time_2 = data_config_bus_data_dummy.minutes_until_passage[i][1];
        uint16_t time_3 = data_config_bus_data_dummy.minutes_until_passage[i][2];

        write_number_to_array(stop, 2 + i, 0, 4);
        write_number_to_array(line, 2 + i, 5, 3);

        // Have to display both in HH:MM format
        if (data_config_bus_data_dummy.minutes_until_passage[i][0] > 99)
        {
            display_time_1[0] = '0' + time_1 / 1000;
            display_time_1[1] = '0' + (time_1 % 1000) / 100;
            display_time_1[2] = 'h';
            display_time_1[3] = '0' + (time_1 % 100) / 10;
            display_time_1[4] = '0' + (time_1 % 10);

            display_time_2[0] = '0' + (time_2 / 1000);
            display_time_2[1] = '0' + ((time_2 % 1000) / 100);
            display_time_2[2] = 'h';
            display_time_2[3] = '0' + ((time_2 % 100) / 10);
            display_time_2[4] = '0' + (time_2 % 10);

            write_string_to_array(display_time_1, 2 + i, 10, 0, 5);
            write_string_to_array(display_time_2, 2 + i, 16, 0, 5);
        }

        // Have to display first in **m and second in HH:MM format
        else if (data_config_bus_data_dummy.minutes_until_passage[i][1] > 99)
        {
            display_time_1[0] = '0' + (time_1 % 100) / 10;
            display_time_1[1] = '0' + (time_1 % 10);
            display_time_1[2] = 'm';

            display_time_2[0] = '0' + (time_2 / 1000);
            display_time_2[1] = '0' + ((time_2 % 1000) / 100);
            display_time_2[2] = 'h';
            display_time_2[3] = '0' + ((time_2 % 100) / 10);
            display_time_2[4] = '0' + (time_2 % 10);

            write_string_to_array(display_time_1, 2 + i, 10, 0, 3);
            write_string_to_array(display_time_2, 2 + i, 14, 0, 5);
        }

        // All times in minutes (**m) format
        else
        {
            display_time_1[0] = '0' + (time_1 % 100) / 10;
            display_time_1[1] = '0' + (time_1 % 10);
            display_time_1[2] = 'm';

            display_time_2[0] = '0' + (time_2 % 100) / 10;
            display_time_2[1] = '0' + (time_2 % 10);
            display_time_2[2] = 'm';

            // Guard against 99+ minutes in third position
            if (data_config_bus_data_dummy.minutes_until_passage[i][2] <= 99)
            {
                display_time_3[0] = '0' + (time_3 % 100) / 10;
                display_time_3[1] = '0' + (time_3 % 10);
                display_time_3[2] = 'm';
            }

            write_string_to_array(display_time_1, 2 + i, 10, 0, 3);
            write_string_to_array(display_time_2, 2 + i, 14, 0, 3);
            write_string_to_array(display_time_3, 2 + i, 18, 0, 3);
        }
    }
}

//==============================================================================
void draw_cursor(uint8_t row)
//
//==============================================================================
{
    for (unsigned char i = 0; i < CURSOR_LENGTH; i++)
    {
        text_under_cursor[i] = local_frame[row][CURSOR_START_COLUMN + i];
    }
    write_string_to_array(cursor, row, CURSOR_START_COLUMN, 0, CURSOR_LENGTH);
}

//==============================================================================
void restore_under_cursor(uint8_t row)
//
//==============================================================================
{
    write_string_to_array(text_under_cursor, row, CURSOR_START_COLUMN, 0, CURSOR_LENGTH);
}

//==============================================================================
// void character_buffer_to_pixel_buffer(const uint8_t character_buf[8][22])
void character_buffer_to_pixel_buffer(void)
//
//==============================================================================
{
    if (render_flag == false)
    {
        return;
    }

    for (unsigned char charact_row = 0; charact_row < CHARACT_ARRAY_ROWS; charact_row++)
    {
        for (unsigned char charact_column = 0; charact_column < CHARACT_ARRAY_COLUMNS; charact_column++)
        {
            uint8_t caract = local_frame[charact_row][charact_column];
            const uint8_t *glyph = get_glyph(caract);

            for (unsigned char glyph_row = 0; glyph_row < BOX_PIXEL_HEIGHT; glyph_row++)
            {
                uint8_t glyph_row_bits = glyph[glyph_row];
                uint8_t pixel_row = charact_row * BOX_PIXEL_HEIGHT + glyph_row;
                for (unsigned char glyph_column = 0; glyph_column < BOX_PIXEL_WIDTH; glyph_column++)
                {
                    uint8_t pixel_column = MARGIN + charact_column * BOX_PIXEL_WIDTH + glyph_column;
                    pixel_buffer_1[pixel_row][pixel_column] = (bool)((glyph_row_bits >> (7 - glyph_column)) & 1);
                }
            }
        }
    }
    pixel_buffer_to_frame_buffer();
}

//==============================================================================
void pixel_buffer_to_frame_buffer(void)
//
//==============================================================================
{
    for (unsigned char pixel_row = 0; pixel_row < PIXEL_BUFFER_ROWS; pixel_row++)
    {
        for (unsigned char pixel_column = 0; pixel_column < PIXEL_BUFFER_COLUMNS; pixel_column++)
        {
            if (pixel_buffer_1[pixel_row][pixel_column] == 1)
            {
                frame_buffer_1[((pixel_row * 128) + pixel_column)] = pixel_coloring(pixel_column, pixel_row);
            }
            else
            {
                frame_buffer_1[((pixel_row * 128) + pixel_column)] = 0x0000;
            }
        }
    }
}

//==============================================================================
uint16_t pixel_coloring(unsigned char x, unsigned char y)
//
//==============================================================================
{
    uint8_t x0 = MARGIN + local_frame[8][0] * BOX_PIXEL_WIDTH;
    uint8_t x1 = MARGIN + (local_frame[8][2] + 1) * BOX_PIXEL_WIDTH - 1;
    uint8_t y0 = local_frame[8][1] * BOX_PIXEL_HEIGHT;
    uint8_t y1 = (local_frame[8][3] + 1) * BOX_PIXEL_HEIGHT - 1;

    if(y <= 7)
    {
        //First row coloring
        return GRAY;
    }
    else if (local_frame[8][0] != 0)
    {
        if (((x >= x0) && (x <= x1)) && ((y >= y0) && (y <= y1)))
        {
            return ACCENT_COLOR;
        }
        else
        {
            return REGULAR_COLOR;
        }
    }
    else
    {
        return REGULAR_COLOR;
    }
}