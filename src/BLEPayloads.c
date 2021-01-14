#include "BLEPayloads.h"
#include "myGPIO.h"
#include <string.h>


void prepReadCustomBytes(prepare_type_env_t *_prepare_read_env, int _bytesLen, const uint8_t *_bytes)
{
    if(_prepare_read_env->prepare_buf == NULL)
    {
        uint8_t dummyRsp[_bytesLen];
        for (int i = 0; i<_bytesLen; i++)
        {
            dummyRsp[i] = _bytes[i];
        }
        _prepare_read_env->prepare_buf = pvPortMalloc(sizeof(dummyRsp));
        _prepare_read_env->prepare_len = sizeof(dummyRsp);
        memcpy(_prepare_read_env->prepare_buf, &dummyRsp, _prepare_read_env->prepare_len);
    }
}

void prepReadDummyBytes(prepare_type_env_t *_prepare_read_env, int _bytes)
{

    if(_prepare_read_env->prepare_buf == NULL)
    {
        uint8_t dummyRsp[_bytes];
        _prepare_read_env->prepare_buf = pvPortMalloc(sizeof(dummyRsp));
        _prepare_read_env->prepare_len = sizeof(dummyRsp);
        memcpy(_prepare_read_env->prepare_buf, &dummyRsp, _prepare_read_env->prepare_len);
    }
}

void prepReadCCCD(prepare_type_env_t *_prepare_read_env, uint16_t _cccd)
{
    if(_prepare_read_env->prepare_buf == NULL)
    {
        _prepare_read_env->prepare_buf = pvPortMalloc(sizeof(uint16_t));
        _prepare_read_env->prepare_len = sizeof(uint16_t);
        memcpy(_prepare_read_env->prepare_buf, &_cccd, _prepare_read_env->prepare_len);
    }
}

void prepReadFlexSensors(prepare_type_env_t *_prepare_read_env)
{
    if(_prepare_read_env->prepare_buf == NULL)
    {
        uint8_t flexSensor[FINGERS];
        flexSensor[THUMB] = (uint8_t)getFingerFlexChannel(THUMB_FLEX_CHANNEL);
        flexSensor[INDEX] = (uint8_t)getFingerFlexChannel(INDEX_FLEX_CHANNEL);
        flexSensor[MIDDLE] = (uint8_t)getFingerFlexChannel(MIDDLE_FLEX_CHANNEL);
        flexSensor[RING] = (uint8_t)getFingerFlexChannel(RING_FLEX_CHANNEL);
        flexSensor[LITTLE] = (uint8_t)getFingerFlexChannel(LITTLE_FLEX_CHANNEL);
        _prepare_read_env->prepare_buf = pvPortMalloc(sizeof(flexSensor));
        _prepare_read_env->prepare_len = sizeof(flexSensor);
        memcpy(_prepare_read_env->prepare_buf, flexSensor, _prepare_read_env->prepare_len); 
    }
}

void prepReadGestures(prepare_type_env_t *_prepare_read_env, gesture_t *_gesture, uint8_t *_gesturesPayload)
{
    if(_prepare_read_env->prepare_buf == NULL)
    {   
        analyzeGestures(_gesture, _gesturesPayload);
        _prepare_read_env->prepare_buf = pvPortMalloc(sizeof(uint8_t));
        _prepare_read_env->prepare_len = sizeof(uint8_t);
        memcpy(_prepare_read_env->prepare_buf, _gesturesPayload, _prepare_read_env->prepare_len);
    }
}

void prepReadFBLed(prepare_type_env_t *_prepare_read_env)
{
    if(_prepare_read_env->prepare_buf == NULL)
    {
        uint8_t fbLed;
        fbLed = (   (uint8_t)gpio_get_level(FB_LED_BLUE_PIN) << FB_LED_BLUE_PLOAD_BIT |
                    (uint8_t)gpio_get_level(FB_LED_GREEN_PIN) << FB_LED_GREEN_PLOAD_BIT |
                    (uint8_t)gpio_get_level(FB_LED_RED_PIN) << FB_LED_RED_PLOAD_BIT );
        _prepare_read_env->prepare_buf = pvPortMalloc(sizeof(uint8_t));
        _prepare_read_env->prepare_len = sizeof(uint8_t);
        memcpy(_prepare_read_env->prepare_buf, &fbLed, _prepare_read_env->prepare_len); 
    }
}

void prepReadBatteryLevel(prepare_type_env_t *_prepare_read_env)
{
    if(_prepare_read_env->prepare_buf == NULL)
    {
        uint8_t batteryLevel = (uint8_t)getBatteryLevel();
        _prepare_read_env->prepare_buf = pvPortMalloc(sizeof(uint8_t));
        _prepare_read_env->prepare_len = sizeof(uint8_t);
        memcpy(_prepare_read_env->prepare_buf, &batteryLevel, _prepare_read_env->prepare_len); 
    }
}

void prepReadBLELog(prepare_type_env_t *_prepare_read_env, char *_string)
{
    if(_prepare_read_env->prepare_buf == NULL)
    {
        _prepare_read_env->prepare_buf = pvPortMalloc(strlen(_string));
        _prepare_read_env->prepare_len = strlen(_string);
        memcpy(_prepare_read_env->prepare_buf, _string, _prepare_read_env->prepare_len);
    }
}

void discardPayload(prepare_type_env_t *_prepare)
{
    if(_prepare->prepare_buf != NULL)
    {
        vPortFree(_prepare->prepare_buf);
        _prepare->prepare_buf = NULL;
        _prepare->prepare_len = 0;
    }
}
