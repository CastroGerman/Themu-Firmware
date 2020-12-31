#include "myTasks.h"
#include "myGPIO.h"
#include "myTimers.h"
#include "MPU6050.h"
#include "myBLE.h"
#include "configs.h"

xTaskHandle thGPIO = NULL, thG0Timer0 = NULL, thG0Timer1 = NULL, thMPU6050 = NULL, thBLE = NULL, thIMUAnalyst = NULL;

void InitTasks(void)
{
    xTaskCreatePinnedToCore(tGPIO, (const char *) "tGPIO",              configMINIMAL_STACK_SIZE*5, NULL, (tskIDLE_PRIORITY + 1UL), &thGPIO, 1);
    xTaskCreatePinnedToCore(tG0Timer0, (const char *) "tG0Timer0",      configMINIMAL_STACK_SIZE*5, NULL, (tskIDLE_PRIORITY + 1UL), &thG0Timer0, 1);
    xTaskCreatePinnedToCore(tG0Timer1, (const char *) "tG0Timer1",      configMINIMAL_STACK_SIZE*5, NULL, (tskIDLE_PRIORITY + 1UL), &thG0Timer1, 1);
    
    #ifdef ENABLE_THEMU_IMU
    xTaskCreatePinnedToCore(tMPU6050, (const char *) "tMPU6050",        configMINIMAL_STACK_SIZE*10, NULL, (tskIDLE_PRIORITY + 1UL), &thMPU6050, 1);
    //xTaskCreatePinnedToCore(tIMUAnalyst, (const char *) "tIMUAnalyst",  configMINIMAL_STACK_SIZE*10, NULL, (tskIDLE_PRIORITY + 2UL), &thIMUAnalyst, 1);
    #endif
    
    #ifdef ENABLE_THEMU_BLE
    xTaskCreatePinnedToCore(tBLE, (const char *) "tBLE",                configMINIMAL_STACK_SIZE*10, NULL, (tskIDLE_PRIORITY + 1UL), &thBLE, 0);
    #endif
}

/**Currently three sleep-related features in ESP-IDF:
 * - FreeRTOS idle task will use 'waiti' instruction to put the CPU into somewhat lower power mode until an interrupt occurs. This happens on both cores independently, and can not be configured.
 * - WiFi/BT driver has "modem sleep" feature which powers down parts of radio and baseband between transmit/receive events. This feature is not related to CPUs.
 * - Deep sleep mode powers down the digital part of the chip; only RTC peripherals, ULP coprocessor, and RTC memories may be powered on in this mode. This is a SoC-level feature, and both CPUs, IRAM, DRAM will be powered off.
 * 
 * There will be more sleep-related features in the next version of ESP-IDF, in particular dynamic frequency switching and light sleep. Both of these features are also SoC-level, and can not be used for one specific CPU.
 * */