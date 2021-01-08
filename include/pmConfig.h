#ifndef PMCONFIG_H_
#define PMCONFIG_H_

#include <esp_err.h>

#define SWITCH_FREQ_TRIES           10
#define SWITCH_FREQ_WAITING_TIME_MS 10

esp_err_t switchCPUFreq(int _mhz);
esp_err_t powerManagementConfig(void);

#endif