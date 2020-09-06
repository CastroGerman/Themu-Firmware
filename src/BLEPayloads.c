#include "BLEPayloads.h"
#include "myBLE.h"
#include <string.h>

void prepReadDummy(void)
{
    uint8_t dummy_rsp = 0;
    a_prepare_read_env.prepare_buf = pvPortMalloc(sizeof(dummy_rsp));
    a_prepare_read_env.prepare_len = sizeof(dummy_rsp);
    memcpy(a_prepare_read_env.prepare_buf, &dummy_rsp, a_prepare_read_env.prepare_len);
}

void prepReadCCCD(uint16_t _cccd)
{
    a_prepare_read_env.prepare_buf = pvPortMalloc(sizeof(uint16_t));
    a_prepare_read_env.prepare_len = sizeof(uint16_t);
    memcpy(a_prepare_read_env.prepare_buf, &_cccd, a_prepare_read_env.prepare_len);
}

void prepReadADC1Channel(adc1_channel_t _channel)
{
    uint8_t adcRead = (uint8_t) readPorcentualADC1Channel(_channel);
    a_prepare_read_env.prepare_buf = pvPortMalloc(sizeof(adcRead));
    a_prepare_read_env.prepare_len = sizeof(adcRead);
    memcpy(a_prepare_read_env.prepare_buf, &adcRead, a_prepare_read_env.prepare_len); 
}

void prepReadGPIOLevel(gpio_num_t _gpioNum)
{
    int value = gpio_get_level(_gpioNum);
    a_prepare_read_env.prepare_buf = pvPortMalloc(sizeof(int));
    a_prepare_read_env.prepare_len = sizeof(int);
    memcpy(a_prepare_read_env.prepare_buf, &value, a_prepare_read_env.prepare_len);
}

void prepReadQuaternion(quaternion_t *_quaternion)
{
    a_prepare_read_env.prepare_buf = pvPortMalloc(sizeof(_quaternion->value));
    a_prepare_read_env.prepare_len = sizeof(_quaternion->value);
    memcpy(a_prepare_read_env.prepare_buf, _quaternion->value, a_prepare_read_env.prepare_len);
}
