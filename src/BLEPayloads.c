#include "BLEPayloads.h"
#include "myBLE.h"
#include <string.h>

void prepReadDummyBytes(int _bytes)
{
    if(a_prepare_read_env.prepare_buf == NULL)
    {
        uint8_t dummyRsp[_bytes];
        a_prepare_read_env.prepare_buf = pvPortMalloc(sizeof(dummyRsp));
        a_prepare_read_env.prepare_len = sizeof(dummyRsp);
        memcpy(a_prepare_read_env.prepare_buf, &dummyRsp, a_prepare_read_env.prepare_len);
    }
}

void prepReadCCCD(uint16_t _cccd)
{
    if(a_prepare_read_env.prepare_buf == NULL)
    {
        a_prepare_read_env.prepare_buf = pvPortMalloc(sizeof(uint16_t));
        a_prepare_read_env.prepare_len = sizeof(uint16_t);
        memcpy(a_prepare_read_env.prepare_buf, &_cccd, a_prepare_read_env.prepare_len);
    }
}

void prepReadADC1Channel(adc1_channel_t _channel)
{
    if(a_prepare_read_env.prepare_buf == NULL)
    {
        uint8_t adcRead = (uint8_t) readPorcentualADC1Channel(_channel);
        a_prepare_read_env.prepare_buf = pvPortMalloc(sizeof(adcRead));
        a_prepare_read_env.prepare_len = sizeof(adcRead);
        memcpy(a_prepare_read_env.prepare_buf, &adcRead, a_prepare_read_env.prepare_len); 
    }
}

void prepReadFlexSensors(void)
{
    if(a_prepare_read_env.prepare_buf == NULL)
    {
        uint8_t flexSensor[5];
        flexSensor[0] = (uint8_t)readPorcentualADC1Channel(FLEX1_CHANNEL);
        flexSensor[1] = (uint8_t)readPorcentualADC1Channel(FLEX2_CHANNEL);
        flexSensor[2] = (uint8_t)readPorcentualADC1Channel(FLEX3_CHANNEL);
        flexSensor[3] = (uint8_t)readPorcentualADC1Channel(FLEX4_CHANNEL);
        flexSensor[4] = (uint8_t)readPorcentualADC1Channel(FLEX5_CHANNEL);
        a_prepare_read_env.prepare_buf = pvPortMalloc(sizeof(flexSensor));
        a_prepare_read_env.prepare_len = sizeof(flexSensor);
        memcpy(a_prepare_read_env.prepare_buf, flexSensor, a_prepare_read_env.prepare_len); 
    }
}

void prepReadGPIOLevel(gpio_num_t _gpioNum)
{
    if(a_prepare_read_env.prepare_buf == NULL)
    {
        int value = gpio_get_level(_gpioNum);
        a_prepare_read_env.prepare_buf = pvPortMalloc(sizeof(int));
        a_prepare_read_env.prepare_len = sizeof(int);
        memcpy(a_prepare_read_env.prepare_buf, &value, a_prepare_read_env.prepare_len);
    }
}

void prepReadQuaternion(quaternion_t *_quaternion)
{
    if(a_prepare_read_env.prepare_buf == NULL)
    {
        a_prepare_read_env.prepare_buf = pvPortMalloc(sizeof(_quaternion->value));
        a_prepare_read_env.prepare_len = sizeof(_quaternion->value);
        memcpy(a_prepare_read_env.prepare_buf, _quaternion->value, a_prepare_read_env.prepare_len);
    }
}

void prepReadFBLed(void)
{
    if(a_prepare_read_env.prepare_buf == NULL)
    {
        uint8_t fbLed;
        fbLed = (   (uint8_t)gpio_get_level(FB_LED_BLUE_PIN) << 2 |
                    (uint8_t)gpio_get_level(FB_LED_GREEN_PIN) << 1 |
                    (uint8_t)gpio_get_level(FB_LED_RED_PIN)     );
        a_prepare_read_env.prepare_buf = pvPortMalloc(sizeof(fbLed));
        a_prepare_read_env.prepare_len = sizeof(fbLed);
        memcpy(a_prepare_read_env.prepare_buf, &fbLed, a_prepare_read_env.prepare_len); 
    }
}

void prepReadBatteryLevel(void)
{
    if(a_prepare_read_env.prepare_buf == NULL)
    {
        uint8_t batteryLevel;
        batteryLevel = (uint8_t)readPorcentualADC1Channel(BATT_CHANNEL);
        a_prepare_read_env.prepare_buf = pvPortMalloc(sizeof(batteryLevel));
        a_prepare_read_env.prepare_len = sizeof(batteryLevel);
        memcpy(a_prepare_read_env.prepare_buf, &batteryLevel, a_prepare_read_env.prepare_len); 
    }
}

void prepReadBLELog(char * _string)
{
    if(a_prepare_read_env.prepare_buf == NULL)
    {
        a_prepare_read_env.prepare_buf = pvPortMalloc(strlen(_string));
        a_prepare_read_env.prepare_len = strlen(_string);
        memcpy(a_prepare_read_env.prepare_buf, _string, a_prepare_read_env.prepare_len);
    }
}

void discardActualPayload(void)
{
    if(a_prepare_read_env.prepare_buf != NULL)
    {
        vPortFree(a_prepare_read_env.prepare_buf);
        a_prepare_read_env.prepare_buf = NULL;
        a_prepare_read_env.prepare_len = 0;
    }
}
