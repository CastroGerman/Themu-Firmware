#ifndef BLEPAYLOADS_H_
#define BLEPAYLOADS_H_

#include "myGPIO.h"
#include "QuaternionLib.h"

void prepReadDummyBytes(int _bytes);
void prepReadCCCD(uint16_t _cccd);
void prepReadADC1Channel(adc1_channel_t _channel);
void prepReadFlexSensors(void);
void prepReadGPIOLevel(gpio_num_t _gpioNum);
void prepReadQuaternion(quaternion_t *_quaternion);
void prepReadFBLed(void);
void prepReadBatteryLevel(void);
void prepReadBLELog(char * _string);
void discardActualPayload(void);

#endif