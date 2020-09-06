#ifndef MYGPIO_H_
#define MYGPIO_H_

#include <driver/gpio.h>
#include <driver/adc.h>
#include <esp_adc_cal.h>

#define DEFAULT_VREF    1100        //[mV]. Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES   64          //Multisampling
#define ADC_CAL_MAX     (int)4095   //To be defined in the calibration process into a struct.
#define ADC_CAL_MIN     (int)0
#define FB_LED_PIN      GPIO_NUM_2

void InitGPIO (void);
void InitADC1 (void);
int readPorcentualADC1Channel(adc1_channel_t _channel);
void readADC1_delete(void);
void IRAM_ATTR gpio16_isr_handler (void *pv);
void tGPIO (void *pv);


#endif /* MYGPIO_H_ */