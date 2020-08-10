#ifndef MYGPIO_H_
#define MYGPIO_H_

#include <driver/gpio.h>
#include <driver/adc.h>
#include <esp_adc_cal.h>

#define DEFAULT_VREF    1100        //[mV]. Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES   64          //Multisampling

void InitGPIO (void);
void InitADC1 (void);
void readADC1(void);
void IRAM_ATTR gpio16_isr_handler (void *pv);
void tGPIO (void *pv);


#endif /* MYGPIO_H_ */