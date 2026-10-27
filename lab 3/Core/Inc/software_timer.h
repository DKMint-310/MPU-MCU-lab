#ifndef SOFTWARE_TIMER_H
#define SOFTWARE_TIMER_H

#include <stdint.h>

extern volatile int timer_flag;
extern volatile int timer2_flag;

/* Call once before starting TIM2, using its actual interrupt interval. */
void timerInit(uint32_t tick_ms);

/* Duration is in milliseconds. Zero stops and resets the timer.
 * Rearming an expired timer preserves overdue time to prevent drift.
 * To discard overdue time for a new task, call setTimer(0) first. */
void setTimer(uint32_t duration_ms);
/* Independent timer for held-button auto-repeat. */
void setTimer2(uint32_t duration_ms);

/* Call in each TIM2 interrupt. Handle timer_flag in the main loop. */
void timerRun(void);

#endif
