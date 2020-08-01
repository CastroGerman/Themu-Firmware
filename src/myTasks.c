#include "myTasks.h"
#include "myGPIO.h"
#include "myTimers.h"
#include "MPU6050.h"

xTaskHandle thGPIO = NULL, thG0Timer0 = NULL, thG0Timer1 = NULL, thMPU6050 = NULL;

void InitTasks(void)
{
    xTaskCreate(tGPIO, (const char *) "tGPIO",              configMINIMAL_STACK_SIZE*10, NULL, (tskIDLE_PRIORITY + 2UL), &thGPIO);
    xTaskCreate(tG0Timer0, (const char *) "tG0Timer0",      configMINIMAL_STACK_SIZE*10, NULL, (tskIDLE_PRIORITY + 1UL), &thG0Timer0);
    xTaskCreate(tG0Timer1, (const char *) "tG0Timer1",      configMINIMAL_STACK_SIZE*10, NULL, (tskIDLE_PRIORITY + 1UL), &thG0Timer1);
    xTaskCreate(tMPU6050, (const char *) "tMPU6050",        configMINIMAL_STACK_SIZE*30, NULL, (tskIDLE_PRIORITY + 1UL), &thMPU6050);
}

