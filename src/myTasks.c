#include "myGPIO.h"
#include "myTasks.h"
#include "myI2C.h"
#include "myTimers.h"

#include "MadgwickAHRS.h"
#include "math.h"


xTaskHandle thGPIO = NULL, thPeriodic = NULL, thPeriodic1 = NULL, thMPU6050 = NULL;

void CrearTareas(void)
{
    xTaskCreate(tGPIO, (const char *) "tGPIO", configMINIMAL_STACK_SIZE*10, NULL, (tskIDLE_PRIORITY + 2UL), &thGPIO);
    xTaskCreate(tPeriodic, (const char *) "tPeriodic", configMINIMAL_STACK_SIZE*10, NULL, (tskIDLE_PRIORITY + 1UL), &thPeriodic);
    xTaskCreate(tPeriodic1, (const char *) "tPeriodic1", configMINIMAL_STACK_SIZE*10, NULL, (tskIDLE_PRIORITY + 1UL), &thPeriodic1);
    
    //xTaskCreate(tMPU6050, (const char *) "tMPU6050", configMINIMAL_STACK_SIZE*10, NULL, (tskIDLE_PRIORITY + 1UL), &thMPU6050);
}



void tGPIO (void *pv)
{
    uint32_t notifycount = 0;
    while (1)
    {
        notifycount = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);//con pdTRUE actua como semaforo binario, con pdFALSE como semaforo contador
        if(notifycount == 1)
        {
            //printf("Interrumpio GPIO 2 \n");            
        }
        else
        {
            //printf("TIMEOUT esperando notificacion en TASK1\n");
        }

    }
}

void tPeriodic (void *pv)
{
    uint32_t notifycount = 0;
    while (1)
    {
        notifycount = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);//con pdTRUE actua como semaforo binario, con pdFALSE como semaforo contador
        if(notifycount == 1)
        {

            //printf("Notificacion recibida en TASK2 del timer\n");
            if(gpio_get_level(GPIO_NUM_2))//recordar que no se pueden leer OUTPUTS, solo INPUTS pins.
            {
                gpio_set_level(GPIO_NUM_18, 0);
            }
            else
            {
                gpio_set_level(GPIO_NUM_18, 1);
            }
            

            printf("Las cuentas al entrar a IRQ Timer0: %lld\n", myTimer[0].timer_counter_value);
            timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &myTimer[0].timer_counter_value);
            printf("Las cuentas en la tarea Timer0: %lld\n", myTimer[0].timer_counter_value);
       
        }
        else
        {
            printf("TIMEOUT esperando notificacion en TASK2\n");
        }

    }
    
}



void tPeriodic1 (void *pv)
{
    uint32_t notifycount = 0;
    while (1)
    {
        notifycount = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);//con pdTRUE actua como semaforo binario, con pdFALSE como semaforo contador
        if(notifycount == 1)
        {
            printf("Las cuentas al entrar a IRQ Timer1: %lld\n", myTimer[1].timer_counter_value);
            timer_get_counter_value(TIMER_GROUP_0, TIMER_1, &myTimer[1].timer_counter_value);
            printf("Las cuentas en la tarea Timer1: %lld\n", myTimer[1].timer_counter_value);
            

        }
        else
        {
            printf("TIMEOUT esperando notificacion en periodic1\n");
        }

    }
    
}


/*
Master Write or Read?

After sending a slave’s address, see step 3 on pictures above, the master either writes to or reads from the slave. The information what the master will actually do is hidden in the least significant bit of the slave’s address.

Therefore the command link instructing the slave that the master will write the data contains the address like (ESP_SLAVE_ADDR << 1) | I2C_MASTER_WRITE and looks as follows:
*/
void tMPU6050 (void *pv)
{
  	uint8_t data[14];
	short accel_x, accel_y, accel_z, temp, gyro_x, gyro_y, gyro_z;
    double gyro_x_offset, gyro_y_offset, gyro_z_offset;

    double faccel_x, faccel_y, faccel_z, ftemp, fgyro_x, fgyro_y, fgyro_z;

    gyro_offset_calibration(&gyro_x_offset,&gyro_y_offset,&gyro_z_offset);


    i2c_cmd_handle_t cmd;		//command link

	while(1) {
		// Tell the MPU6050 to position the internal register pointer to register
		// MPU6050_ACCEL_XOUT_H.
		cmd = i2c_cmd_link_create();
		ESP_ERROR_CHECK(i2c_master_start(cmd));
		ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (I2C_ADDRESS << 1) | I2C_MASTER_WRITE, 1));
		ESP_ERROR_CHECK(i2c_master_write_byte(cmd, MPU6050_ACCEL_XOUT_H, 1));
		ESP_ERROR_CHECK(i2c_master_stop(cmd));
		ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000/portTICK_PERIOD_MS));
		i2c_cmd_link_delete(cmd);

		cmd = i2c_cmd_link_create();
		ESP_ERROR_CHECK(i2c_master_start(cmd));
		ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (I2C_ADDRESS << 1) | I2C_MASTER_READ, 1));

		ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data,   0));//MPU6050_ACCEL_XOUT_H
		ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+1, 0));//MPU6050_ACCEL_XOUT_L
		ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+2, 0));//MPU6050_ACCEL_YOUT_H
		ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+3, 0));//MPU6050_ACCEL_YOUT_L
		ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+4, 0));//MPU6050_ACCEL_ZOUT_H
		ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+5, 0));//MPU6050_ACCEL_ZOUT_L

		ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+6, 0));//MPU6050_TEMP_OUT_H
		ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+7, 0));//MPU6050_TEMP_OUT_L

		ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+8, 0));//MPU6050_GYRO_XOUT_H
		ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+9, 0));//MPU6050_GYRO_XOUT_L
		ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+10,0));//MPU6050_GYRO_YOUT_H
		ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+11,0));//MPU6050_GYRO_YOUT_L
		ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+12,0));//MPU6050_GYRO_ZOUT_H
		ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+13,1));//MPU6050_GYRO_ZOUT_L 
        //Enviar ACK(0) n-1 veces y terminar con la ultima lectura enviando NAK(1)

		//i2c_master_read(cmd, data, sizeof(data), 1);
		ESP_ERROR_CHECK(i2c_master_stop(cmd));
		ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000/portTICK_PERIOD_MS));
		i2c_cmd_link_delete(cmd);

		accel_x = (data[0] << 8) | data[1];
		accel_y = (data[2] << 8) | data[3];
		accel_z = (data[4] << 8) | data[5];
		temp 	= (data[6] << 8) | data[7];
		gyro_x 	= (data[8] << 8) | data[9];
		gyro_y 	= (data[10] << 8) | data[11];
		gyro_z 	= (data[12] << 8) | data[13];

        // Read gyro values and convert to Radians per second
        fgyro_x  =  (gyro_x - (short)gyro_x_offset)  / 1877.468103f;
        fgyro_y  =  (gyro_y - (short)gyro_y_offset)  / 1877.468103f;
        fgyro_z  =  (gyro_z - (short)gyro_z_offset)  / 1877.468103f;
        
        // Read accel values  and convert relative to 1G
       // faccel_x = (accel_x / G_CALC_VAL )/ 2.0f;
       // faccel_y = (accel_y / G_CALC_VAL )/ 2.0f;
       // faccel_z = (accel_z / G_CALC_VAL )/ 2.0f;

        faccel_x = (accel_x - ACCEL_X_OFF) / G_CALC_VAL;
        faccel_y = (accel_y - ACCEL_Y_OFF) / G_CALC_VAL;
        faccel_z = (accel_z - ACCEL_Z_OFF) / G_CALC_VAL;

        ftemp = temp + 4350.0f;

		//printf("accel_x: %d, accel_y: %d, accel_z: %d | temp: %d | gryo_x: %d, gryo_y: %d, gryo_z: %d\n", (int)accel_x, (int)accel_y, (int)accel_z, (short)temp, (int)gyro_x,(int) gyro_y, (short)gyro_z);
        //printf("faccel_x: %f, faccel_y: %f, faccel_z: %f | ftemp: %f | fgryo_x: %f, fgryo_y: %f, fgryo_z: %f\n", faccel_x, faccel_y, faccel_z, ftemp, fgyro_x, fgyro_y, fgyro_z);
		//printf("fgryo_x_off: %f, fgryo_y_off: %f, fgryo_z_off: %f\n", gyro_x_offset, gyro_y_offset, gyro_z_offset);

        //printf("faccel_x: %f, faccel_y: %f, faccel_z: %f | ftemp: %f | fgryo_x: %f, fgryo_y: %f, fgryo_z: %f\n", faccel_x, faccel_y, faccel_z, ftemp, fgyro_x, fgyro_y, fgyro_z);
        

        //MadgwickAHRSupdateIMU(fgyro_x, fgyro_y, fgyro_z, faccel_x, faccel_y, faccel_z);
        
        //printf("Q0 = %f, Q1 = %f, Q2 = %f, Q3 = %f\n", q0,q1,q2,q3);

       
        Kalman_1D (faccel_x, (double)0.04);

        float accel_ang_x = atan(faccel_x / sqrt(pow(faccel_y, 2) + pow(faccel_z, 2)))*(180.0 / 3.14);
        float accel_ang_y = atan(faccel_y / sqrt(pow(faccel_x, 2) + pow(faccel_z, 2)))*(180.0 / 3.14);
        float accel_ang_z = atan(faccel_z / sqrt(pow(faccel_x, 2) + pow(faccel_y, 2)))*(180.0 / 3.14);
 

        //printf("%f,%f,\n", faccel_x,EST);

        printf("%f,%f,%f\n",accel_ang_x,accel_ang_y,accel_ang_z);

        vTaskDelay(1000/portTICK_PERIOD_MS);
	}
}

/*
void correccion_offset (short accel_x,short accel_y,short accel_z,short temp,short gyro_x,short gyro_y,short gyro_z) 
{
    float gforce_x, gforce_y, gforce_z;
    gforce_x = (accel_x/G_CALC_VAL);
    gforce_y = (accel_y/G_CALC_VAL);
    gforce_z = (accel_z/G_CALC_VAL);
    
}
*/
void gyro_offset_calibration (double *gyro_x_offset,double *gyro_y_offset,double *gyro_z_offset)
{
    uint8_t data[6], cal_iterations;
	i2c_cmd_handle_t cmd;		//command link

    for (cal_iterations = 0; cal_iterations < 64; cal_iterations++)
    {
        cmd = i2c_cmd_link_create();
        ESP_ERROR_CHECK(i2c_master_start(cmd));
        ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (I2C_ADDRESS << 1) | I2C_MASTER_WRITE, 1));
        ESP_ERROR_CHECK(i2c_master_write_byte(cmd, MPU6050_GYRO_XOUT_H, 1));
        ESP_ERROR_CHECK(i2c_master_stop(cmd));
        ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000/portTICK_PERIOD_MS));
        i2c_cmd_link_delete(cmd);
        cmd = i2c_cmd_link_create();
        ESP_ERROR_CHECK(i2c_master_start(cmd));
        ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (I2C_ADDRESS << 1) | I2C_MASTER_READ, 1));
        ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data,0));//MPU6050_GYRO_XOUT_H
        ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+1,0));//MPU6050_GYRO_XOUT_L
        ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+2,0));//MPU6050_GYRO_YOUT_H
        ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+3,0));//MPU6050_GYRO_YOUT_L
        ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+4,0));//MPU6050_GYRO_ZOUT_H
        ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+5,1));//MPU6050_GYRO_ZOUT_L 
        ESP_ERROR_CHECK(i2c_master_stop(cmd));
        ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000/portTICK_PERIOD_MS));
        i2c_cmd_link_delete(cmd);

        *gyro_x_offset += ((data[0] << 8) | data[1]);
        *gyro_y_offset += ((data[2] << 8) | data[3]);
        *gyro_z_offset += ((data[4] << 8) | data[5]);
    }  
    *gyro_x_offset /= 64;
    *gyro_y_offset /= 64;
    *gyro_z_offset /= 64;
}

double EST;
double E_EST;
void Kalman_1D (double MEA, double E_MEA)
{

    //Ganancia de Kalman
    double KG = (E_EST)/(E_EST + E_MEA);
   // printf("KG: %f\n",KG);
    //Valor estimado
    EST = EST + KG*(MEA-EST);

    //Error en la estimacion
    E_EST = (1-KG)*(E_EST);


}