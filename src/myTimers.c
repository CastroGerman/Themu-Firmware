#include "myTimers.h"
#include "myTasks.h"


myTimers_t myTimer[2];
uint64_t cuentat0, cuentat1;

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
    
    if (_timer_index)
    {
        timer_isr_register(_timer_group, _timer_index, g0_timer1_isr_handler, 
        (void *) NULL, ESP_INTR_FLAG_IRAM, NULL);
    }
    else
    {
        timer_isr_register(_timer_group, _timer_index, g0_timer0_isr_handler, 
        (void *) NULL, ESP_INTR_FLAG_IRAM, NULL);
    }
    
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
    TIMERG0.hw_timer[0].update = 1;
    myTimer[0].timer_counter_value = 
    ((uint64_t) TIMERG0.hw_timer[0].cnt_high) << 32 | TIMERG0.hw_timer[0].cnt_low;

    TIMERG0.int_clr_timers.t0 = 1; //Cleaning interrupt flag.
    TIMERG0.hw_timer[0].config.alarm_en = TIMER_ALARM_EN;  //Reload alarm

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

void IRAM_ATTR g0_timer1_isr_handler (void *pv)
{
    TIMERG0.hw_timer[1].update = 1;



    myTimer[1].timer_counter_value = 
    ((uint64_t) TIMERG0.hw_timer[1].cnt_high) << 32 | TIMERG0.hw_timer[1].cnt_low;

    TIMERG0.int_clr_timers.t1 = 1; //Cleaning interrupt flag.
   
    timer_set_counter_value(TIMER_GROUP_0, TIMER_1, 0x00000000ULL); //Why drift?
    /*
    TIMERG0.hw_timer[1].cnt_high = 0;
    TIMERG0.hw_timer[1].cnt_low = 0;
    TIMERG0.hw_timer[1].reload = 1;
    */

    // Solution that doesn't drift:
    //myTimer[1].timer_counter_value += (uint64_t) (G0_TIMER1_INTERVAL_SEC * TIMER_SCALE);
    //TIMERG0.hw_timer[1].alarm_high = (uint32_t) (myTimer[1].timer_counter_value >> 32);
    //TIMERG0.hw_timer[1].alarm_low = (uint32_t) myTimer[1].timer_counter_value;

    TIMERG0.hw_timer[1].config.alarm_en = TIMER_ALARM_EN;  //Reload alarm
    
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(thPeriodic1, &xHigherPriorityTaskWoken);
    if(xHigherPriorityTaskWoken != pdFALSE){}
    
}



