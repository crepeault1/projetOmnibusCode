#ifndef PROCESSENCODERACTIONS_H
#define PROCESSENCODERACTIONS_H

/**
 ******************************************************************************
 * @file           : process_rotary_encoder_actions.h
 * @brief          : 
 * @author         : Samuel Crepeault
 *
 ******************************************************************************
 * @details
 * 
 ******************************************************************************
 */

 /* Includes ------------------------------------------------------------------*/

/* Defines -------------------------------------------------------------------*/

/* Variables ---------------------------------------------------------------- */
typedef struct
{
  uint8_t information;
  uint8_t request;
  uint8_t statut;
} PROCESS_ENCODER_ACTIONS;

extern PROCESS_ENCODER_ACTIONS process_encoder_actions;

/* Prototypes ----------------------------------------------------------------*/
void process_encoder_actions_init(void);
void process_encoder_actions_run(void);

#endif