#include "myTasks.h"
#include "myGPIO.h"
#include "myTimers.h"
#include "myI2C.h"
#include "MPU6050.h"
#include "myBLE.h"
#include "configs.h"
#include <esp_pm.h>

/* app_main is also a task with IDLE+1 priority */
void app_main() 
{
    vTaskPrioritySet(xTaskGetCurrentTaskHandle(), tskIDLE_PRIORITY + 5U);
    InitGPIO();

    #if defined ENABLE_THEMU_ADC
    InitADC1();
    #endif

    InitI2C();
    
    #ifdef ENABLE_THEMU_IMU
    InitMPU6050();
    #endif
    
    #ifdef ENABLE_THEMU_BLE
    InitBLE();
    #endif
    
    InitTimer(TIMER_GROUP_0, TIMER_0, TIMER_AUTORELOAD_EN, G0_TIMER0_INTERVAL_SEC);  
    InitTimer(TIMER_GROUP_0, TIMER_1, TIMER_AUTORELOAD_EN, G0_TIMER1_INTERVAL_SEC);
    timer_start(TIMER_GROUP_0, TIMER_1);
    InitTasks();

    vTaskDelete(xTaskGetCurrentTaskHandle());
}
