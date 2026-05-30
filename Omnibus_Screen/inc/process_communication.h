#ifndef PROCESSCOMMUNICATION_H
#define PROCESSCOMMUNICATION_H
/**
  ******************************************************************************
  * @file           : process_communication.h
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

/* Defines -------------------------------------------------------------------*/
#define NOTHING_TO_DO 0
#define CHECK_STOP 1
#define CHECK_LINE 2
#define GET_LINES 3
#define ACK_LINE_QUANTITY 4
#define GET_DIRECTIONS 5
#define SET_DIRECTION 6
#define GET_STOPS 7
#define ACK_STOP_QUANTITY 8
#define WAIT_TIMES 9

/* Variables ---------------------------------------------------------------- */
typedef struct {
    const char *header;
    int action;
} INCOMING_FRAME_TYPE;

extern uint8_t process_communication_outbound_code;

/* Prototypes ----------------------------------------------------------------*/
void process_communication_init(void);

void process_communication_read(void);
void process_communication_write(void);
void process_communication_interpret_frame(int action);

#endif