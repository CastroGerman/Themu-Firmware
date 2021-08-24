#ifndef GESTURES_H_
#define GESTURES_H_

#include "MPU6050.h"
#include "myGPIO.h"
#include "QuaternionLib.h"

#define XVERSOR     1,0,0
#define XVERSOR_N   -1,0,0
#define YVERSOR     0,1,0
#define ZVERSOR_N   0,0,-1

#define VECTOR_REF  XVERSOR_N

#define GST_POINTING_UP_EVT_TGR     0.707   
#define GST_POINTING_DOWN_EVT_TGR   -0.707   
#define GST_POINTING_FRONT_EVT_TGR  0.707   
#define GST_POINTING_BACK_EVT_TGR   -0.707   
#define GST_POINTING_LEFT_EVT_TGR   0.707   
#define GST_POINTING_RIGHT_EVT_TGR  -0.707  
#define GST_FIST_BUMP_EVT_TGR       -1.8 
#define GST_SLIDE_RIGHT_EVT_TGR     0.8
#define GST_POS_ROLL_EVT_TGR        8
#define GST_NEG_ROLL_EVT_TGR        -8

#define GST_POINTING_UP_PLOAD_BIT       0 
#define GST_POINTING_DOWN_PLOAD_BIT     1   
#define GST_POINTING_FRONT_PLOAD_BIT    2
#define GST_NEG_ROLL_PLOAD_BIT          3   
#define GST_POINTING_BACK_PLOAD_BIT        
#define GST_POINTING_LEFT_PLOAD_BIT     4   
#define GST_POINTING_RIGHT_PLOAD_BIT    5 
#define GST_FIST_BUMP_PLOAD_BIT         6
#define GST_SLIDE_RIGHT_PLOAD_BIT       7
 

typedef struct gesture 
{
    double imu[MPU6050_16BITS_REGS];
    int flex[FINGERS];
    quaternionForm_t quaternion;
}gesture_t;

extern gesture_t *gesture;
extern uint8_t gesturesPayload;

int isPointingUp(vector_t _vector);
int isPointingDown(vector_t _vector);
int isPointingFront(vector_t _vector);
int isPointingBack(vector_t _vector);
int isPointingLeft(vector_t _vector);
int isPointingRight(vector_t _vector);
gesture_t *createGesture(void);
void updateGesture(gesture_t *_gesture, double *_imu, float _q0, float _q1, float _q2, float _q3);
void analyzeGestures(gesture_t *_gesture, uint8_t *_gesturesPayload);
void tGestures (void *pv);
void updateGesture_new(gesture_t *_gesture, MPU6050_data_t *_imu, float _q0, float _q1, float _q2, float _q3);

#endif