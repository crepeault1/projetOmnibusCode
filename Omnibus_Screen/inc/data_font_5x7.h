#ifndef FONT_H
#define FONT_H
/**
 ******************************************************************************
 * @file           : font.h
 * @brief          : Character binary values
 * @author         : Samuel Crepeault
 *
 ******************************************************************************
 * @details
 * Font is 5x7 and is packed in 6x8 boxes. Each box contains a
 *
 *
 * @attention
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/

/* Defines -------------------------------------------------------------------*/
#define FONT_5X7_CELL_WIDTH 6
#define FONT_5X7_CELL_HEIGHT 8
#define FONT_5X7_VISIBLE_WIDTH 5
#define FONT_5X7_VISIBLE_HEIGHT 7
#define FONT_5X7_GLYPH_BYTES 8
#define FONT_5X7_NUM_GLYPHS 136

#define GLYPH_MISSING 0
#define UP_ARROW 130
#define DOWN_ARROW 131
/* Variables ---------------------------------------------------------------- */
extern const uint8_t byte_to_glyph[256];
extern const uint8_t glyph_5x7[FONT_5X7_NUM_GLYPHS][FONT_5X7_GLYPH_BYTES];

/* Functions -----------------------------------------------------------------*/
const uint8_t* get_glyph(uint8_t input_character);

#endif