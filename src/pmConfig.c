#include "pmConfig.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <esp_pm.h>
#include <esp32/clk.h>

esp_err_t switchCPUFreq(int _mhz)
{
    esp_err_t ret;
    esp_pm_config_esp32_t pmConfig = 
    {
        .max_freq_mhz = _mhz,
        .min_freq_mhz = esp_clk_xtal_freq()/1000000
    }; 
    printf("Trying to change CPU frequency to %d MHz.\n", _mhz);
    ret = esp_pm_configure(&pmConfig);
    if(ret != ESP_OK) return ret;
    printf("Waiting for CPU frequency to be set to %d MHz...\n", _mhz);
    for(int i = 0; (esp_clk_cpu_freq()/1000000) != _mhz; i++)
    {
        if(i == SWITCH_FREQ_TRIES)
        {
            printf("TIMEOUT waiting for CPU frequency to be switched.\n");
            return ESP_ERR_TIMEOUT;
        }
        vTaskDelay(SWITCH_FREQ_WAITING_TIME_MS);
    }
    printf("Frequency is set to %d MHz\n", _mhz);
    return ESP_OK;
}

esp_err_t powerManagementConfig(void)
{
    /**
    * Automatic light sleep is based on FreeRTOS Tickless Idle functionality. 
    * If automatic light sleep is requested while the option CONFIG_FREERTOS_USE_TICKLESS_IDLE is not enabled 
    * in menuconfig, esp_pm_configure() will return the error ESP_ERR_NOT_SUPPORTED.
    */
    esp_err_t ret = switchCPUFreq(240);
    if(ret != ESP_OK) ret = switchCPUFreq(160);
    if(ret != ESP_OK) ret = switchCPUFreq(80);

    return ret;    
}