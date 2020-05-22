#include "myTasks.h"
#include "myGPIO.h"
#include "myTimers.h"
#include "myI2C.h"
#include "MPU6050.h"

/* app_main is also a task with IDLE+1 priority */

/*
*   TODO: 
*   Get Angles and Orientations
*/
void app_main() 
{
    EST = 0.0;
    E_EST = 0.06;
  
    InitGPIO();
    InitTimer(TIMER_GROUP_0, TIMER_0, TIMER_AUTORELOAD_EN, G0_TIMER0_INTERVAL_SEC);  
    InitTimer(TIMER_GROUP_0, TIMER_1, TIMER_AUTORELOAD_DIS, G0_TIMER1_INTERVAL_SEC);
    InitI2C();

    InitTasks();
    
    while(1)
    {
        
        E_EST = 0.06;
        vTaskDelay(1000/portTICK_RATE_MS); 
    }

}
