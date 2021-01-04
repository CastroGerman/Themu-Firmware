#ifndef BLEPAYLOADS_H_
#define BLEPAYLOADS_H_

#include "QuaternionLib.h"
#include "Gesture.h"

#define FB_LED_RED_PLOAD_BIT    0
#define FB_LED_GREEN_PLOAD_BIT  1
#define FB_LED_BLUE_PLOAD_BIT   2


void prepReadDummyBytes(int _bytes);
void prepReadCCCD(uint16_t _cccd);
void prepReadFlexSensors(void);
void prepReadQuaternion(quaternion_t *_quaternion);
void prepReadGestures(gesture_t *_gesture);
void prepReadFBLed(void);
void prepReadBatteryLevel(void);
void prepReadBLELog(char * _string);
void discardActualPayload(void);

#endif