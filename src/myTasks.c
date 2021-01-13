#include "myTasks.h"
#include "myGPIO.h"
#include "myTimers.h"
#include "MPU6050.h"
#include "myBLE.h"
#include "Gesture.h"
#include "configs.h"

xTaskHandle thGPIO = NULL, thG0Timer0 = NULL, thG0Timer1 = NULL, thMPU6050 = NULL, thBLE = NULL, thGestures = NULL;

void InitTasks(void)
{
    xTaskCreatePinnedToCore(tGPIO, (const char *) "tGPIO", tGPIO_STACK_SIZE, NULL, tGPIO_PRIORITY, &thGPIO, tGPIO_CORE_ID);
    xTaskCreatePinnedToCore(tG0Timer0, (const char *) "tG0Timer0", tG0TIMER0_STACK_SIZE, NULL, tG0TIMER0_PRIORITY, &thG0Timer0, tG0TIMER0_CORE_ID);
    xTaskCreatePinnedToCore(tG0Timer1, (const char *) "tG0Timer1", tG0TIMER1_STACK_SIZE, NULL, tG0TIMER1_PRIORITY, &thG0Timer1, tG0TIMER1_CORE_ID);
    
    #ifdef ENABLE_THEMU_IMU
    xTaskCreatePinnedToCore(tMPU6050, (const char *) "tMPU6050", tMPU6050_STACK_SIZE, NULL, tMPU6050_PRIORITY, &thMPU6050, tMPU6050_CORE_ID);
    #endif
    
    #ifdef ENABLE_THEMU_BLE
    xTaskCreatePinnedToCore(tBLE, (const char *) "tBLE", tBLE_STACK_SIZE, NULL, tBLE_PRIORITY, &thBLE, tBLE_CORE_ID);
    #endif

    #ifdef ENABLE_THEMU_GESTURES
    xTaskCreatePinnedToCore(tGestures, (const char *) "tGestures", tGESTURES_STACK_SIZE, NULL, tGESTURES_PRIORITY, &thGestures, tGESTURES_CORE_ID);
    #endif

}

/**Currently three sleep-related features in ESP-IDF:
 * - FreeRTOS idle task will use 'waiti' instruction to put the CPU into somewhat lower power mode until an interrupt occurs. This happens on both cores independently, and can not be configured.
 * - WiFi/BT driver has "modem sleep" feature which powers down parts of radio and baseband between transmit/receive events. This feature is not related to CPUs.
 * - Deep sleep mode powers down the digital part of the chip; only RTC peripherals, ULP coprocessor, and RTC memories may be powered on in this mode. This is a SoC-level feature, and both CPUs, IRAM, DRAM will be powered off.
 * 
 * There will be more sleep-related features in the next version of ESP-IDF, in particular dynamic frequency switching and light sleep. Both of these features are also SoC-level, and can not be used for one specific CPU.
 * */