#include "myTasks.h"
#include "myGPIO.h"
#include "myTimers.h"
#include "myI2C.h"

/* app_main is also a task with IDLE+1 priority */

/*
*   TODO: 
*   programar otro timer pero con autoreload disable 
*   para poder tomar la cuenta y usarla en la integral del giroscopio.
*   https://www.luisllamas.es/arduino-orientacion-imu-mpu-6050/
*/
void app_main() 
{
    EST = 0.0;
    E_EST = 0.06;
    InitGPIO();
    InitTimer(TIMER_GROUP_0, TIMER_0, TIMER_AUTORELOAD_EN, G0_TIMER0_INTERVAL_SEC);  
    InitTimer(TIMER_GROUP_0, TIMER_1, TIMER_AUTORELOAD_DIS, G0_TIMER1_INTERVAL_SEC);
    //InitI2C();

    CrearTareas();
    
    int i = 0;
    while(1)
    {
        //timer_get_counter_value(TIMER_GROUP_0,TIMER_0, &cuenta);
        //printf("Entró %d veces al MAIN \n timer count %lld \n \n", i++,cuenta);
        //printf("Entro %d al MAIN\n", i++);
        E_EST = 0.06;
        vTaskDelay(1000/portTICK_RATE_MS); //1s
    }

}
