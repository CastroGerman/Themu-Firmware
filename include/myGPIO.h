#ifndef MYGPIO_H_
#define MYGPIO_H_

#include <driver/gpio.h>
/*
* GPIO 18 defined as output.
* GPIO 2 defined as input.
*/
void InitGPIO (void);
void IRAM_ATTR gpio2_isr_handler (void *pv);
void tGPIO (void *pv);


#endif /* MYGPIO_H_ */