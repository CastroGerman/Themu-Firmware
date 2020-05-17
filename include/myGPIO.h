#ifndef MYGPIO_H_
#define MYGPIO_H_

#include "driver/gpio.h"

#define PIN_OUTPUT  GPIO_SEL_18 //Define de aviso, no implementado
#define PIN_INPUT   GPIO_SEL_2  //Define de aviso, no implementado


void InitGPIO (void);
void IRAM_ATTR gpio2_isr_handler (void *pv);


#endif /* MYGPIO_H_ */