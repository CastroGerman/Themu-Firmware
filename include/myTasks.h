#ifndef MYTASKS_H_
#define MYTASKS_H_

#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#define G_CALC_VAL 16384.0f
#define ACCEL_X_OFF 800.0f
#define ACCEL_Y_OFF 100.0f
#define ACCEL_Z_OFF 3400.0f
extern xSemaphoreHandle sTestSemphr;
extern xTaskHandle thGPIO, thPeriodic;
extern double EST;
extern double E_EST;

void CrearSemaforos(void);
void CrearTareas(void);
void tTestTask (void *pv); 
void tGPIO (void *pv);
void tPeriodic (void *pv);
void tMPU6050 (void *pv);
void gyro_offset_calibration (double *gyro_x_offset,double *gyro_y_offset,double *gyro_z_offset);


void Kalman_1D (double MEA, double E_MEA);





#endif /* MYTASKS_H_ */