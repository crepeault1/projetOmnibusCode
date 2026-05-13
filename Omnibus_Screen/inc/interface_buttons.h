#ifndef INTERFACEBUTTONS_H
#define INTERFACEBUTTONS_H
/**
 ******************************************************************************
 * @file           : interface_buttons.h
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
#define INTERFACEBUTTON_HELD 0
#define INTERFACEBUTTON_RELEASED 1
#define INTERFACEBUTTON_UNKNOWN 2

/* Variables ---------------------------------------------------------------- */
typedef struct
{
  unsigned char information;
  unsigned char button_state;
  unsigned int debounce_counter;
  unsigned int read_counter;
} INTERFACE_BUTTON;

extern INTERFACE_BUTTON interface_button1;
extern INTERFACE_BUTTON interface_button2;
extern INTERFACE_BUTTON interface_button3;

/* Prototypes ----------------------------------------------------------------*/
void interface_button1_init(void);
void interface_button2_init(void);
void interface_button3_init(void);

void interface_button1_run(void);
void interface_button2_run(void);
void interface_button3_run(void);


#endif