#ifndef PROCESSCLOCK_H
#define PROCESSCLOCK_H
/**
 ******************************************************************************
 * @file           : process_clock.h
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

/* Variables ---------------------------------------------------------------- */
typedef struct
{
    uint32_t epoch;
    bool update;
    bool display_on;
    uint32_t awake_until_epoch;
} PROCESS_CLOCK;

extern PROCESS_CLOCK process_clock;
/* Prototypes ----------------------------------------------------------------*/
void process_clock_init(void);
void process_clock_handle(void);

#endif