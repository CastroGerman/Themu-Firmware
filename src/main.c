#include "myTasks.h"
#include "myGPIO.h"
#include "myTimers.h"
#include "myI2C.h"
#include "MPU6050.h"
#include "myBLE.h"

/* app_main is also a task with IDLE+1 priority */
void app_main() 
{
    vTaskPrioritySet(xTaskGetCurrentTaskHandle(), tskIDLE_PRIORITY + 5U);
    InitGPIO();
    InitADC1();
    InitI2C();
    InitMPU6050();
    InitBLE();
    InitTimer(TIMER_GROUP_0, TIMER_0, TIMER_AUTORELOAD_EN, G0_TIMER0_INTERVAL_SEC);  
    InitTimer(TIMER_GROUP_0, TIMER_1, TIMER_AUTORELOAD_EN, G0_TIMER1_INTERVAL_SEC);
    InitTasks();

    vTaskDelete(xTaskGetCurrentTaskHandle());
}
