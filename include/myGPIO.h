#ifndef MYGPIO_H_
#define MYGPIO_H_

#include <driver/gpio.h>
#include <driver/adc.h>
#include <esp_adc_cal.h>

#define DEFAULT_VREF    1100        //[mV]. Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES   64          //Multisampling
#define ADC_CAL_MAX     (int)4100   //To be defined in the calibration process into a struct.
#define ADC_CAL_MIN     (int)2600
#define FB_LED_PIN      GPIO_NUM_2

#define LED_ON  1
#define LED_OFF 0

#define FB_LED_RED_PIN      GPIO_NUM_27
#define FB_LED_GREEN_PIN    GPIO_NUM_26
#define FB_LED_BLUE_PIN     GPIO_NUM_12
#define BUTTON_PIN          GPIO_NUM_25

#define FLEX1_CHANNEL   ADC1_CHANNEL_3
#define FLEX2_CHANNEL   ADC1_CHANNEL_6
#define FLEX3_CHANNEL   ADC1_CHANNEL_7
#define FLEX4_CHANNEL   ADC1_CHANNEL_4
#define FLEX5_CHANNEL   ADC1_CHANNEL_5
#define BATT_CHANNEL    ADC1_CHANNEL_0

#define FLEX1_CHANNEL_ATT   ADC_ATTEN_DB_6//ADC_ATTEN_DB_0
#define FLEX2_CHANNEL_ATT   ADC_ATTEN_DB_6//ADC_ATTEN_DB_2_5
#define FLEX3_CHANNEL_ATT   ADC_ATTEN_DB_6
#define FLEX4_CHANNEL_ATT   ADC_ATTEN_DB_6//ADC_ATTEN_DB_11
#define FLEX5_CHANNEL_ATT   ADC_ATTEN_DB_6
#define BATT_CHANNEL_ATT    ADC_ATTEN_DB_6

void InitGPIO (void);
void InitADC1 (void);
int readPorcentualADC1Channel(adc1_channel_t _channel);
void readADC1_delete(void);
void IRAM_ATTR glove_button_isr_handler (void *pv);
void tGPIO (void *pv);

#endif /* MYGPIO_H_ */