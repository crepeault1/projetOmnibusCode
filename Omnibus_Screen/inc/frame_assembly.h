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
extern uint8_t frame_start[8][21];
extern uint8_t frame_menu[8][21];
extern uint8_t frame_test0[8][21];
extern uint8_t frame_test1[8][21];
extern uint8_t frame_test2[8][21];
extern uint8_t frame_test3[8][21];

/* Fonctions -----------------------------------------------------------------*/
void character_buffer_to_pixel_buffer(const uint8_t character_buf[8][21]);
void pixel_buffer_to_frame_buffer(void);

#endif