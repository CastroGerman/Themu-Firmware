#ifndef MYTASKS_H_
#define MYTASKS_H_

#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"


extern xSemaphoreHandle sTestSemphr;
extern xTaskHandle thGPIO, thPeriodic;

void CrearSemaforos(void);
void CrearTareas(void);
void tTestTask (void *pv); 
void tGPIO (void *pv);
void tPeriodic (void *pv);
void tMPU6050 (void *pv);






#endif /* MYTASKS_H_ */