#include "myTasks.h"
#include "myGPIO.h"
#include "myTimers.h"
#include "myI2C.h"
/*
*   app_main es una task tambien de prioridad idle+1
*/
void app_main() 
{
    //InitGPIO();
    //InitTimer(TIMER_GROUP_0, TIMER_0, TIMER_AUTORELOAD_EN, G0_TIMER0_INTERVAL_SEC);  
    InitI2C();
    //CrearSemaforos();
    CrearTareas();
    
    int i = 0;
    while(1)
    {
        //timer_get_counter_value(TIMER_GROUP_0,TIMER_0, &cuenta);
        //printf("Entró %d veces al MAIN \n timer count %lld \n \n", i++,cuenta);
        printf("Entro %d al MAIN\n", i++);
        vTaskDelay(5000/portTICK_RATE_MS); //1s
    }

}
