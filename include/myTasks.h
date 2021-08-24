#ifndef MYTASKS_H_
#define MYTASKS_H_

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define tGPIO_PRIORITY      (tskIDLE_PRIORITY + 1UL)
#define tG0TIMER0_PRIORITY  (tskIDLE_PRIORITY + 1UL)
#define tG0TIMER1_PRIORITY  (tskIDLE_PRIORITY + 1UL)
#define tMPU6050_PRIORITY   (tskIDLE_PRIORITY + 1UL)
#define tBLE_PRIORITY       (tskIDLE_PRIORITY + 1UL)
#define tGESTURES_PRIORITY  (tskIDLE_PRIORITY + 2UL)
#define tMAIN_PRIORITY      (tskIDLE_PRIORITY + 3UL)

#define tGPIO_STACK_SIZE        configMINIMAL_STACK_SIZE
#define tG0TIMER0_STACK_SIZE    configMINIMAL_STACK_SIZE
#define tG0TIMER1_STACK_SIZE    configMINIMAL_STACK_SIZE
#define tMPU6050_STACK_SIZE     configMINIMAL_STACK_SIZE*4
#define tBLE_STACK_SIZE         configMINIMAL_STACK_SIZE*4
#define tGESTURES_STACK_SIZE    configMINIMAL_STACK_SIZE*4

#define tGPIO_CORE_ID       1
#define tG0TIMER0_CORE_ID   1
#define tG0TIMER1_CORE_ID   1
#define tMPU6050_CORE_ID    1 
#define tBLE_CORE_ID        0
#define tGESTURES_CORE_ID   1


extern xTaskHandle thGPIO, thG0Timer0, thG0Timer1, thMPU6050, thBLE, thGestures;


void InitTasks (void);

#endif /* MYTASKS_H_ */