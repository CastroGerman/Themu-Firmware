#include "myI2C.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

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
	
	vTaskDelay(200/portTICK_PERIOD_MS);	
}