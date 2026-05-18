#ifndef FRAMEASSEMBLY_H
#define FRAMEASSEMBLY_H
/**
 ******************************************************************************
 * @file           : frame_assembly.h
 * @brief          : 
 * @author         : Samuel Crepeault
 *
 ******************************************************************************
 * @details
 * 
 *
 *
 * @attention
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/

/* Defines -------------------------------------------------------------------*/

/* Variables ---------------------------------------------------------------- */
extern uint16_t __attribute__((aligned(4))) frame_buffer_1[8192];
extern uint16_t __attribute__((aligned(4))) frame_buffer_2[8192];
extern uint8_t text_under_cursor[4];

/* Fonctions -----------------------------------------------------------------*/
void frame_assembly_init(void);

void character_array_copy(const uint8_t character_buf[8][22]);
void write_character_to_array(uint8_t character, uint8_t row, uint8_t column);
void write_string_to_array(uint8_t str[], uint8_t row, uint8_t column, uint8_t length);

void draw_cursor(uint8_t row);
void restore_under_cursor(uint8_t row);

void character_buffer_to_pixel_buffer(void);
void pixel_buffer_to_frame_buffer(void);

#endif