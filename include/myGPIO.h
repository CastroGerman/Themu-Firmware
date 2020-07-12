#ifndef MYGPIO_H_
#define MYGPIO_H_

#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

extern xTaskHandle thGPIO;

/*
* GPIO 18 defined as output.
* GPIO 2 defined as input.
*/
void InitGPIO (void);
void IRAM_ATTR gpio2_isr_handler (void *pv);
void tGPIO (void *pv);


#endif /* MYGPIO_H_ */