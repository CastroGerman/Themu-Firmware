#ifndef GESTURES_H_
#define GESTURES_H_

#include "MPU6050.h"
#include "myGPIO.h"
#include "QuaternionLib.h"

typedef struct gesture 
{
    double imu[MPU6050_16BITS_REGS];
    int flex[FINGERS];
    quaternionForm_t quaternion;
}gesture_t;

//extern gesture_t *gesture;

gesture_t *createGesture(void);
void updateGesture(gesture_t *_gesture, double *_imu, float _q0, float _q1, float _q2, float _q3);
void tGestures (void *pv);

#endif