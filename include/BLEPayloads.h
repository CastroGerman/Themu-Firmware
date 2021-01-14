#ifndef BLEPAYLOADS_H_
#define BLEPAYLOADS_H_

#include "myBLE.h"
#include "Gesture.h"



void prepReadCustomBytes(prepare_type_env_t *_prepare_read_env, int _bytesLen, const uint8_t *_bytes);
void prepReadDummyBytes(prepare_type_env_t *_prepare_read_env, int _bytes);
void prepReadCCCD(prepare_type_env_t *_prepare_read_env, uint16_t _cccd);
void prepReadFlexSensors(prepare_type_env_t *_prepare_read_env);
void prepReadGestures(prepare_type_env_t *_prepare_read_env, gesture_t *_gesture, uint8_t *_gesturesPayload);
void prepReadFBLed(prepare_type_env_t *_prepare_read_env);
void prepReadBatteryLevel(prepare_type_env_t *_prepare_read_env);
void prepReadBLELog(prepare_type_env_t *_prepare_read_env, char *_string);
void discardPayload(prepare_type_env_t *_prepare);

#endif