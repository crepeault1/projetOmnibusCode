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
#include "font_5x7.h"
#include "frame_assembly.h"

/* Defines -------------------------------------------------------------------*/
#define BOX_PIXEL_WIDTH 6
#define BOX_PIXEL_HEIGHT 8
#define CHARACT_ARRAY_ROWS 8
#define CHARACT_ARRAY_COLUMNS 21
#define PIXEL_BUFFER_ROWS 64
#define PIXEL_BUFFER_COLUMNS 128

#define MARGIN 1

/* Variables ---------------------------------------------------------------- */
uint8_t frame_start[8][21] = {
    {"                     "},
    {"       OMNIBUS       "},
    {"                     "},
    {"   Informations de   "},
    {"       transit       "},
    {"                     "},
    {"     ver. 1.0.0      "},
    {"                     "}};

bool pixel_buffer_1[PIXEL_BUFFER_ROWS][PIXEL_BUFFER_COLUMNS];
bool pixel_buffer_2[PIXEL_BUFFER_ROWS][PIXEL_BUFFER_COLUMNS];

uint16_t __attribute__((aligned(4))) frame_buffer_1[8192];
uint16_t __attribute__((aligned(4))) frame_buffer_2[8192];
/* Fonctions -----------------------------------------------------------------*/

//==============================================================================
void character_buffer_to_pixel_buffer(const uint8_t character_buf[8][21])
//
//==============================================================================
{
    for (unsigned char charact_row = 0; charact_row < CHARACT_ARRAY_ROWS; charact_row++)
    {
        for (unsigned char charact_column = 0; charact_column < CHARACT_ARRAY_COLUMNS; charact_column++)
        {
            uint8_t caract = character_buf[charact_row][charact_column];
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
                frame_buffer_1[((pixel_row * 128) + pixel_column)] = 0xFFFF;
            }
            else
            {
                frame_buffer_1[((pixel_row * 128) + pixel_column)] = 0x0000;
            }
        }
    }
}