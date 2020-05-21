#ifndef MYTASKS_H_
#define MYTASKS_H_

#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"


extern xTaskHandle thGPIO, thPeriodic, thPeriodic1;
extern double EST;
extern double E_EST;

void CrearSemaforos(void);
void CrearTareas(void);
void tTestTask (void *pv); 
void tGPIO (void *pv);
void tPeriodic (void *pv);
void tPeriodic1 (void *pv);
void tMPU6050 (void *pv);
void gyro_offset_calibration (double *gyro_x_offset,double *gyro_y_offset,double *gyro_z_offset);
void offsetCalibration (double *accel_x_offset, double *accel_y_offset, double *accel_z_offset, 
double *temp_offset, double *gyro_x_offset,double *gyro_y_offset,double *gyro_z_offset);

void Kalman_1D (double MEA, double E_MEA);





#endif /* MYTASKS_H_ */