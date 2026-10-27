#include "software_timer.h"
#include "main.h"

volatile int timer_flag = 0;
static volatile int timer_running = 0;
static volatile int64_t timer_counter = 0;
static uint32_t timer_tick_ms = 10U;

volatile int timer2_flag = 0;
static volatile int timer2_running = 0;
static volatile int64_t timer2_counter = 0;

void timerInit(uint32_t tick_ms)
{
  uint32_t interrupt_state = __get_PRIMASK();
  __disable_irq();
  timer_tick_ms = tick_ms;
  timer2_counter = 0;
  timer2_flag = 0;
  timer2_running = 0;
  timer_counter = 0;
  timer_flag = 0;
  timer_running = 0;
  __set_PRIMASK(interrupt_state);
}

void setTimer(uint32_t duration_ms)
{
  uint32_t interrupt_state = __get_PRIMASK();
  __disable_irq();

  if (duration_ms == 0U || timer_tick_ms == 0U)
  {
    timer_counter = 0;
    timer_flag = 0;
    timer_running = 0;
  }
  else
  {
    if (timer_flag == 1)
    {
      timer_counter += duration_ms;
    }
    else
    {
      timer_counter = duration_ms;
    }
    timer_running = 1;
    timer_flag = (timer_counter <= 0);
  }

  __set_PRIMASK(interrupt_state);
}

void setTimer2(uint32_t duration_ms)
{
  uint32_t interrupt_state = __get_PRIMASK();
  __disable_irq();

  if (duration_ms == 0U || timer_tick_ms == 0U)
  {
    timer2_counter = 0;
    timer2_flag = 0;
    timer2_running = 0;
  }
  else
  {
    if (timer2_flag == 1)
    {
      timer2_counter += duration_ms;
    }
    else
    {
      timer2_counter = duration_ms;
    }
    timer2_running = 1;
    timer2_flag = (timer2_counter <= 0);
  }

  __set_PRIMASK(interrupt_state);
}

void timerRun(void)
{
  if (timer_running == 1)
  {
    /* Continue while flagged, so main-loop delays are retained. */
    timer_counter -= timer_tick_ms;
    if (timer_counter <= 0)
    {
      timer_flag = 1;
    }
  }
  if (timer2_running == 1)
  {
    timer2_counter -= timer_tick_ms;
    if (timer2_counter <= 0) timer2_flag = 1;
  }

}
