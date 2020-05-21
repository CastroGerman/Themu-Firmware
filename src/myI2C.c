#include "myI2C.h"
#include "MPU6050.h"

void InitI2C (void)
{
	i2c_config_t config;
	config.mode = I2C_MODE_MASTER;
	config.sda_io_num = PIN_SDA;
	config.scl_io_num = PIN_CLK;
	config.sda_pullup_en = GPIO_PULLUP_ENABLE;
	config.scl_pullup_en = GPIO_PULLUP_ENABLE;
	config.master.clk_speed = MASTER_CLK_SPEED;
	ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &config));
	ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0));

	i2c_cmd_handle_t cmd;		//command link
	vTaskDelay(200/portTICK_PERIOD_MS);

	//configuro el power managment. Armo la trama.
	cmd = i2c_cmd_link_create();
	ESP_ERROR_CHECK(i2c_master_start(cmd)); //Cargo bit de start
	ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (I2C_ADDRESS << 1) | I2C_MASTER_WRITE, 1)); //cargo slave address
	i2c_master_write_byte(cmd, MPU6050_PWR_MGMT_1, 1); //dato
	i2c_master_write_byte(cmd, 0, 1); 
	ESP_ERROR_CHECK(i2c_master_stop(cmd)); //Cargo bit de stop
	i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000/portTICK_PERIOD_MS); //envio trama y bloqueo un tiempo hasta terminar
	i2c_cmd_link_delete(cmd); //libero los recursos utilizados por cmd.

	//Seteo escala de giroscopio a 2000 grados/seg
	cmd = i2c_cmd_link_create();
	ESP_ERROR_CHECK(i2c_master_start(cmd)); 
	ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (I2C_ADDRESS << 1) | I2C_MASTER_WRITE, 1)); 
	i2c_master_write_byte(cmd, MPU6050_GYRO_CONFIG, 1); //dato
	i2c_master_write_byte(cmd, FS_SEL<<3 , 1); 
	ESP_ERROR_CHECK(i2c_master_stop(cmd)); 
	i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000/portTICK_PERIOD_MS); 
	i2c_cmd_link_delete(cmd); 

	//Seteo escala de acelerometro a +- 2g
	cmd = i2c_cmd_link_create();
	ESP_ERROR_CHECK(i2c_master_start(cmd)); 
	ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (I2C_ADDRESS << 1) | I2C_MASTER_WRITE, 1)); 
	i2c_master_write_byte(cmd, MPU6050_ACCEL_CONFIG, 1); //dato
	i2c_master_write_byte(cmd, AFS_SEL<<3 , 1); 
	ESP_ERROR_CHECK(i2c_master_stop(cmd)); 
	i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000/portTICK_PERIOD_MS); 
	i2c_cmd_link_delete(cmd); 

}