#ifndef INTERFACEENCODER_H
#define INTERFACEENCODER_H
/**
 ******************************************************************************
 * @file           : interface_rotary_encoder.h
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
#define INTERFACEENCODER_HELD 0
#define INTERFACEENCODER_RELEASED 1
#define INTERFACEENCODER_UNKNOWN 2

/* Variables ---------------------------------------------------------------- */
typedef struct
{
  //Rotary encoder
  unsigned long encoder_last_count;
  unsigned char encoder_direction;
  unsigned char encoder_information;
  unsigned int encoder_read_counter;
  
  //Encoder button
  unsigned char button_state;
  unsigned char button_information;
  unsigned int button_debounce_counter;
  unsigned int button_read_counter;
} INTERFACE_ENCODER;

extern INTERFACE_ENCODER interface_encoder;

/* Prototypes ----------------------------------------------------------------*/
void interface_encoder_init(void);
void interface_encoder_button_run(void);
void interface_encoder_run(void);

#endif