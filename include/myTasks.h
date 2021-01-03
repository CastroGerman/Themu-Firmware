#ifndef MYTASKS_H_
#define MYTASKS_H_

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

extern xTaskHandle thGPIO, thG0Timer0, thG0Timer1, thMPU6050, thBLE, thGestures;

void InitTasks (void);

#endif /* MYTASKS_H_ */