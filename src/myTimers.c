#include "myTimers.h"
#include "myTasks.h"


void InitTimer (int _timer_group, int _timer_index, bool _auto_reload, double _timer_interval_sec)
{

    /* Select and initialize basic parameters of the timer */
    timer_config_t config;
    config.divider          = TIMER_DIVIDER;
    config.counter_dir      = TIMER_COUNT_UP;
    config.counter_en       = TIMER_PAUSE;
    config.alarm_en         = TIMER_ALARM_DIS;
    config.intr_type        = TIMER_INTR_LEVEL;
    config.auto_reload      = _auto_reload;
    timer_init(_timer_group, _timer_index, &config);

    /* Timer's counter will initially start from value below.
       Also, if auto_reload is set, this value will be automatically reload on alarm */
    timer_set_counter_value(_timer_group, _timer_index, 0x00000000ULL);

    /* Configure the alarm value and the interrupt on alarm. */
    timer_set_alarm_value(_timer_group, _timer_index, _timer_interval_sec * TIMER_SCALE);
    
    timer_isr_register(_timer_group, _timer_index, g0_timer0_isr_handler, 
        (void *) NULL, ESP_INTR_FLAG_IRAM, NULL);


    timer_set_alarm(_timer_group, _timer_index, TIMER_ALARM_EN);
    timer_enable_intr(_timer_group, _timer_index);
    timer_start(_timer_group, _timer_index);
}
/*
pxHigherPriorityTaskWoken: vTaskNotifyGiveFromISR() will set *pxHigherPriorityTaskWoken 
to pdTRUE if sending the notification caused the task to which the notification was
sent to leave the Blocked state, and the unblocked task has a priority higher 
than the currently running task. If vTaskNotifyGiveFromISR() sets this value 
to pdTRUE then a context switch should be requested before the interrupt is exited. 
How a context switch is requested from an ISR is dependent on the port - see 
the documentation page for the port in use.
*/
void IRAM_ATTR g0_timer0_isr_handler (void *pv)
{

    TIMERG0.int_clr_timers.t0 = 1; //Limpio flag de interrupcion
    TIMERG0.hw_timer[0].config.alarm_en = TIMER_ALARM_EN;  //recargo alarma

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(thPeriodic, &xHigherPriorityTaskWoken);
    if(xHigherPriorityTaskWoken != pdFALSE)
    {
        /* If xHigherPriorityTaskWoken is now set to pdTRUE then a context switch
        should be performed to ensure the interrupt returns directly to the highest
        priority task.  The macro used for this purpose is dependent on the port in
        use and may be called portEND_SWITCHING_ISR(). */
        //portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        // We can force a context switch here.  Context switching from an
        // ISR uses port specific syntax.  Check the demo task for your port
        // to find the syntax required.
        //vTaskSwitchContext(); revisar bien como.
    }
    
}
