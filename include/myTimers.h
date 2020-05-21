#ifndef MYTIMERS_H_
#define MYTIMERS_H_

#include "esp_types.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"

/*
*   El CLK del core corre a 80MHz
*/

#define TIMER_DIVIDER           16  //  Hardware timer clock divider (revisar bien los valores admisibles)
#define TIMER_SCALE             (TIMER_BASE_CLK / TIMER_DIVIDER)  // convert counter value to seconds
#define G0_TIMER0_INTERVAL_SEC  (3.00) // sample test interval for the first timer
#define G0_TIMER1_INTERVAL_SEC  (2.00)

typedef struct myTimers
{
    uint64_t timer_counter_value;
} myTimers_t;

extern myTimers_t myTimer[2];


void InitTimer (int _timer_group, int _timer_index, bool _auto_reload, double _timer_interval_sec);
void IRAM_ATTR g0_timer0_isr_handler (void *pv);
void IRAM_ATTR g0_timer1_isr_handler (void *pv);

extern uint64_t cuentat0;
extern uint64_t cuentat1;



#endif