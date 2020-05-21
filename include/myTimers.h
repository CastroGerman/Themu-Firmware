#ifndef MYTIMERS_H_
#define MYTIMERS_H_

#include <esp_types.h>
#include <driver/periph_ctrl.h>
#include <driver/timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

/*
*   Core CLK base running at 80MHz
*/

#define TIMER_DIVIDER           16  //  Hardware timer clock divider (revisar bien los valores admisibles)
#define TIMER_SCALE             (TIMER_BASE_CLK / TIMER_DIVIDER)  // convert counter value to seconds
#define G0_TIMER0_INTERVAL_SEC  (0.50) // sample test interval for the first timer
#define G0_TIMER1_INTERVAL_SEC  (5.00)

typedef struct myTimers
{
    uint64_t timer_counter_value;
} myTimers_t;

extern myTimers_t myTimer[2];
extern xTaskHandle thG0Timer0, thG0Timer1;

void InitTimer (int _timer_group, int _timer_index, bool _auto_reload, double _timer_interval_sec);
void IRAM_ATTR g0_timer0_isr_handler (void *pv);
void IRAM_ATTR g0_timer1_isr_handler (void *pv);
void tG0Timer0 (void *pv);
void tG0Timer1 (void *pv);


#endif