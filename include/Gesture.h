#ifndef GESTURES_H_
#define GESTURES_H_

#include "MPU6050.h"
#include "myGPIO.h"
#include "QuaternionLib.h"


#define XVERSOR     1,0,0
#define YVERSOR     0,1,0
#define ZVERSOR     0,0,1

#define VECTOR_REF  XVERSOR

#define GST_POINTING_UP_EVT_TGR     0.8   
#define GST_POINTING_DOWN_EVT_TGR   -0.8   
#define GST_POINTING_FRONT_EVT_TGR  0.8   
#define GST_POINTING_BACK_EVT_TGR   -0.8   
#define GST_POINTING_LEFT_EVT_TGR   0.8   
#define GST_POINTING_RIGHT_EVT_TGR  -0.8   

#define GST_POINTING_UP_PLOAD_BIT       0 
#define GST_POINTING_DOWN_PLOAD_BIT     1   
#define GST_POINTING_FRONT_PLOAD_BIT    2   
#define GST_POINTING_BACK_PLOAD_BIT     3   
#define GST_POINTING_LEFT_PLOAD_BIT     4   
#define GST_POINTING_RIGHT_PLOAD_BIT    5  

typedef struct gesture 
{
    double imu[MPU6050_16BITS_REGS];
    int flex[FINGERS];
    quaternionForm_t quaternion;
}gesture_t;

extern gesture_t *gesture;

int isPointingUp(vector_t _vector);
int isPointingDown(vector_t _vector);
int isPointingFront(vector_t _vector);
int isPointingBack(vector_t _vector);
int isPointingLeft(vector_t _vector);
int isPointingRight(vector_t _vector);
gesture_t *createGesture(void);
void updateGesture(gesture_t *_gesture, double *_imu, float _q0, float _q1, float _q2, float _q3);
uint8_t analyzeGestures(gesture_t *_gesture);
void tGestures (void *pv);

#endif