#include "myGPIO.h"
#include "myTasks.h"
#include "myI2C.h"

xSemaphoreHandle sTestSemphr;     
xTaskHandle thGPIO = NULL, thPeriodic = NULL, thMPU6050 = NULL;

void CrearSemaforos(void)
{
    vSemaphoreCreateBinary(sTestSemphr);
    //xSemaphoreTake(sTestSemphr,(portTickType) 1); //esta no le esta gustando por algun motivo.
}

void CrearTareas(void)
{
    //xTaskCreate(tGPIO, (const char *) "tGPIO", configMINIMAL_STACK_SIZE*10, NULL, (tskIDLE_PRIORITY + 2UL), &thGPIO);
    //xTaskCreate(tPeriodic, (const char *) "tPeriodic", configMINIMAL_STACK_SIZE*10, NULL, (tskIDLE_PRIORITY + 1UL), &thPeriodic);
    //xTaskCreate(tTestTask, (const char *) "tTestTask", configMINIMAL_STACK_SIZE*10, NULL,    (tskIDLE_PRIORITY + 1UL), (xTaskHandle *) NULL);
    xTaskCreate(tMPU6050, (const char *) "tMPU6050", configMINIMAL_STACK_SIZE*10, NULL, (tskIDLE_PRIORITY + 1UL), &thMPU6050);
}


void tTestTask (void *pv)
{
    
    static int i = 0;
    while(1)
    {
        //xSemaphoreTake(sTick,portMAX_DELAY);
        //gpio_set_level(GPIO_NUM_18, i%2);
        printf("Entró %d veces al TEST \n", i++);
        vTaskDelay(1000/portTICK_RATE_MS); //1s
    }
}


void tGPIO (void *pv)
{
    uint32_t notifycount = 0;
    while (1)
    {
        notifycount = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);//con pdTRUE actua como semaforo binario, con pdFALSE como semaforo contador
        if(notifycount == 1)
        {
            printf("Interrumpio GPIO 2 \n");            
        }
        else
        {
            printf("TIMEOUT esperando notificacion en TASK1\n");
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
                printf("Apagando GPIO 18\n");
                gpio_set_level(GPIO_NUM_18, 0);
            }
            else
            {
                printf("Prendiendo GPIO 18\n");
                gpio_set_level(GPIO_NUM_18, 1);
            }
            
        }
        else
        {
            printf("TIMEOUT esperando notificacion en TASK2\n");
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

		printf("accel_x: %d, accel_y: %d, accel_z: %d | temp: %d | gryo_x: %d, gryo_y: %d, gryo_z: %d\n", accel_x, accel_y, accel_z, temp, gyro_x, gyro_y, gyro_z);
		vTaskDelay(100/portTICK_PERIOD_MS);
	}
}