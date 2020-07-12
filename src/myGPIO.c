#include "myGPIO.h"

xTaskHandle thGPIO = NULL;

void InitGPIO (void)
{
    gpio_config_t io_config;

    //disable interrupt
    io_config.intr_type = GPIO_PIN_INTR_DISABLE;
    //set as output mode
    io_config.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_config.pin_bit_mask = GPIO_SEL_18;
    //disable pull-down mode
    io_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    //disable pull-up mode
    io_config.pull_up_en = GPIO_PULLUP_DISABLE;
    //configure GPIO with the given settings
    gpio_config(&io_config);

    //interrupt of rising edge
    io_config.intr_type = GPIO_PIN_INTR_NEGEDGE;
    //bit mask of the pins, use GPIO4/5 here
    io_config.pin_bit_mask = GPIO_SEL_2;
    //set as input mode
    io_config.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_config.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_config);
    

    //change gpio intrrupt type for one pin
    //gpio_set_intr_type(GPIO_SEL_2, GPIO_INTR_ANYEDGE);

    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_EDGE);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_NUM_2, gpio2_isr_handler, (void*) NULL);

}


void IRAM_ATTR gpio2_isr_handler (void *pv)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(thGPIO, &xHigherPriorityTaskWoken);
    if(xHigherPriorityTaskWoken != pdFALSE){}
}

void tGPIO (void *pv)
{
    uint32_t notifycount = 0;
    while (1)
    {
        notifycount = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);//pdTRUE = as a binary semaphore. pdFALSE = as a counting semaphore.
        if(notifycount == 1)
        {
            //printf("Interrumpio GPIO 2 \n");            
        }
        else
        {
            printf("TIMEOUT esperando notificacion en tGPIO\n");
        }

    }
}