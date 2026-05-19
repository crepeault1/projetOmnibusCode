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
#include "data_UI.h"

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
    scheduler_phase_array[PHASE_PROCESS_FRAME_ASSEMBLY] = character_buffer_to_pixel_buffer;
}

//==============================================================================
void character_array_copy(const uint8_t character_buf[8][22], uint8_t accent_reg[4])
//
//==============================================================================
{
    memcpy(local_frame, character_buf, sizeof(uint8_t) * 176);
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
void write_string_to_array(uint8_t str[], uint8_t row, uint8_t column, uint8_t length)
//
//==============================================================================
{
    if (column + length > CHARACT_ARRAY_COLUMNS)
    {
        length = CHARACT_ARRAY_COLUMNS - column;
    }
    for (unsigned char i = 0; i < length; i++)
    {
        local_frame[row][column + i] = str[i];
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
    write_string_to_array(cursor, row, CURSOR_START_COLUMN, CURSOR_LENGTH);
}

//==============================================================================
void restore_under_cursor(uint8_t row)
//
//==============================================================================
{
    write_string_to_array(text_under_cursor, row, CURSOR_START_COLUMN, CURSOR_LENGTH);
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

    if (local_frame[8][0] != 0)
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