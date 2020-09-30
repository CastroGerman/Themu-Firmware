#include "BLEPayloads.h"
#include "myBLE.h"
#include <string.h>

void prepReadDummy(void)
{
    if(a_prepare_read_env.prepare_buf == NULL)
    {
        uint8_t dummyRsp = 0;
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
        flexSensor[0] = (uint8_t)readPorcentualADC1Channel(ADC_CHANNEL_3);
        flexSensor[1] = (uint8_t)readPorcentualADC1Channel(ADC_CHANNEL_4);
        flexSensor[2] = (uint8_t)readPorcentualADC1Channel(ADC_CHANNEL_5);
        flexSensor[3] = (uint8_t)readPorcentualADC1Channel(ADC_CHANNEL_6);
        flexSensor[4] = (uint8_t)readPorcentualADC1Channel(ADC_CHANNEL_7);
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

void prepReadBLELog(char * _string)
{
    if(a_prepare_read_env.prepare_buf == NULL)
    {
        a_prepare_read_env.prepare_buf = pvPortMalloc(strlen(_string));
        a_prepare_read_env.prepare_len = strlen(_string);
        memcpy(a_prepare_read_env.prepare_buf, _string, a_prepare_read_env.prepare_len);
    }
}
