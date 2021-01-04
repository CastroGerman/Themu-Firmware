#ifndef MYBLE_H_
#define MYBLE_H_

#include "myTasks.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "esp_bt.h" //implements BT controller and VHCI configuration procedures from the host side.
#include "esp_gap_ble_api.h" //implements GAP configuration, such as advertising and connection parameters.
#include "esp_gatts_api.h" //implements GATT configuration, such as creating services and characteristics.
#include "esp_bt_defs.h"
#include "esp_bt_main.h" //implements initialization and enabling of the Bluedroid stack.
#include "esp_gatt_common_api.h"
#include "configs.h"

#define PAYLOAD_LEN 22

#define FLEX_SENSOR_SERVICE_UUID    0x0100
#define FLEX_SENSOR_CHAR_UUID       0x0110
#define FLEX_SENSOR_DESCR_UUID      0x2902 
#define RESTART_SERVICE_UUID        0x0200
#define RESTART_CHAR_UUID           0x0210
#define RESTART_DESCR_UUID          0x0211
#define GESTURES_SERVICE_UUID     0x0300
#define GESTURES_CHAR_UUID        0x0310
#define GESTURES_DESCR_UUID       0x2902
#define FB_LED_SERVICE_UUID         0x0400
#define FB_LED_CHAR_UUID            0x0410
#define FB_LED_DESCR_UUID           0x0411
#define BATTERY_SERVICE_UUID        0x180F
#define BATTERY_CHAR_UUID           0x2A19
#define BATTERY_DESCR_UUID          0x2901
#define BATTERY_DESCR2_UUID         0x2902
/*The handles are:
Service handle
Characteristic handle
Characteristic value handle
Characteristic descriptor handle*/
/*When you use esp_ble_gatts_create_service there is last argument where you set handles count for this service. In example it is 4. 
* I found that for every characteristic use 2 handles as service and descriptor use 1 handler. 
* So if you like to add characteristic you should init service with 6 or more handle number*/
#define FLEX_SENSOR_NUM_HANDLE      4
#define RESTART_NUM_HANDLE          4
#define GESTURES_NUM_HANDLE       4
#define FB_LED_NUM_HANDLE           4
#define BATTERY_NUM_HANDLE          5
#define BASE_SERVICE_HANDLE         40

#ifdef ENABLE_THEMU_BLE_LOGS
    #define LOG_SERVICE_UUID        0x0600
    #define LOG_CHAR_UUID           0x0610
    #define LOG_DESCR_UUID          0x2902
    #define LOG_NUM_HANDLE          4
#endif

#define TEST_DEVICE_NAME    "GATTS_THEMU"
#define GATTS_TAG           "GATTS_THEMU_INFO"

#define PREPARE_BUF_MAX_SIZE 1024

#define adv_config_flag      (1 << 0)
#define scan_rsp_config_flag (1 << 1)
#define CONFIG_SET_RAW_ADV_DATA

#define PROFILE_NUM         1
#define PROFILE_A_APP_ID    0


#define INDICATIONS_ENABLE      0x0002
#define NOTIFICATION_ENABLE     0x0001
#define NOTIFICATION_DISABLE    0x0000

enum Profile_handlers {
    flex_sensor_handle = BASE_SERVICE_HANDLE,
    flex_sensor_char_handle,
    flex_sensor_charvalue_handle,
    flex_sensor_descr_handle,
    restart_handle,
    restart_char_handle,
    restart_charvalue_handle,
    restart_descr_handle,
    gestures_handle,
    gestures_char_handle,
    gestures_charvalue_handle,
    gestures_descr_handle,
    fb_led_handle,
    fb_led_char_handle,
    fb_led_charvalue_handle,
    fb_led_descr_handle,
    battery_handle,
    battery_char_handle,
    battery_charvalue_handle,
    battery_descr_handle,
    battery_descr2_handle
    #ifdef ENABLE_THEMU_BLE_LOGS
    ,log_handle
    ,log_char_handle
    ,log_charvalue_handle
    ,log_descr_handle
    #endif
};

struct gatts_profile_inst {
    esp_gatts_cb_t gatts_cb;
    uint16_t gatts_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_handle;
    esp_gatt_srvc_id_t service_id;
    uint16_t char_handle;
    esp_bt_uuid_t char_uuid;
    esp_gatt_perm_t perm;
    esp_gatt_char_prop_t property;
    uint16_t descr_handle;
    esp_bt_uuid_t descr_uuid;
};

typedef struct {
    uint8_t                 *prepare_buf;
    int                     prepare_len;
} prepare_type_env_t;

typedef struct{
    uint16_t flex_sensor, restart, gestures, fb_led, battery;
    #ifdef ENABLE_THEMU_BLE_LOGS
    uint16_t ble_log;
    #endif
} cccd_t; /*!< Client Characteristic Configuration Descriptor */

#ifdef ENABLE_THEMU_BLE_LOGS
extern char *bleLogMsg;
#endif

extern prepare_type_env_t a_prepare_write_env, a_prepare_read_env;
extern esp_gatt_if_t a_gatts_if;
extern uint16_t a_conn_id;
extern cccd_t a_cccd;

/**Statics:
 * 
 * 
 * Functions:
 * void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
 * void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
 * void example_write_event_env(esp_gatt_if_t gatts_if, prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param);
 * void example_exec_write_event_env(prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param);
 * void gatts_profile_a_write_handle(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
 * void gatts_profile_a_read_handle(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
 * void gatts_profile_a_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
 */

void InitBLE(void);
uint16_t getCCCD(esp_ble_gatts_cb_param_t *param);
void disableAllNotifications(void);
void tBLE (void *pv);

#endif /* MYBLE_H_ */