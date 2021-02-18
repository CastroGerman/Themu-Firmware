#include "myBLE.h"
#include "MPU6050.h"
#include "myGPIO.h"
#include "BLEPayloads.h"
#include <string.h> //memcpy & memset
#include "configs.h"
#include "myTimers.h"
#include "Gesture.h"

/*Declare the static functions & variables*/
static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
static void gatts_profile_a_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

static esp_gatt_char_prop_t a_property = 0;
static uint8_t adv_config_done = 0;

#ifdef CONFIG_SET_RAW_ADV_DATA
/**You can find the advertising data types (AD Type) in:
 * https://www.bluetooth.com/specifications/assigned-numbers/generic-access-profile/
 */
static uint8_t raw_adv_data[] = {
    /*  LEN , TYPE, VALUE   */
        0x02, 0x01, 0x06,       /*0x01 = Flags*/
        0x02, 0x0a, 0xeb,       /*0x0A = Tx Power Level*/
        #ifdef ENABLE_THEMU_BLE_LOGS
        0x0d, 0x03, 0x00, 0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x04, 0x0f, 0x18, 0x00, 0x06  /*0x03 = Complete List of 16-bit Service Class UUIDs*/
        #else
        0x0b, 0x03, 0x00, 0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x04, 0x0f, 0x18  /*0x03 = Complete List of 16-bit Service Class UUIDs*/
        #endif

        };
static uint8_t raw_scan_rsp_data[] = {
        0x06, 0x09, 0x54, 0x48, 0x45, 0x4d, 0x55 /*0x09 = Complete Local Name*/
};
#endif /* CONFIG_SET_RAW_ADV_DATA */

/*These parameters configure the advertising interval between 40 ms to 80 ms. 
* The advertisement is ADV_IND, which is a generic, not directed
* to a particular central device and connectable type. 
* The address type is public, uses all channels and allows both scan and 
* connection requests from any central.*/
/*The advertising data is the information that is shown to the client, while 
* the advertising parameters are the configuration required by the GAP to execute.*/
static esp_ble_adv_params_t adv_params = {
    .adv_int_min        = 0x20,
    .adv_int_max        = 0x40,
    .adv_type           = ADV_TYPE_IND,
    .own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
    //.peer_addr            =
    //.peer_addr_type       =
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

/* One gatt-based profile one app_id and one gatts_if, this array will store the gatts_if returned by ESP_GATTS_REG_EVT */
static struct gatts_profile_inst gl_profile_tab[PROFILE_NUM] = {
    [PROFILE_A_APP_ID] = {
        .gatts_cb = gatts_profile_a_event_handler,
        .gatts_if = ESP_GATT_IF_NONE,       /* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
    },
};

#ifdef ENABLE_THEMU_BLE_LOGS
char *bleLogMsg;
#endif

/*Saving APP parameters to handle notifications*/
gloveProfile_t *a;

/*
prepare_type_env_t a_prepare_write_env, a_prepare_read_env;
esp_gatt_if_t a_gatts_if;
uint16_t a_conn_id;
cccd_t a_cccd;
uint8_t bleAbleToSend = 1;
*/

gloveProfile_t *createGloveProfile (void)
{
    gloveProfile_t *gloveProf = pvPortMalloc(sizeof(gloveProfile_t));
    gloveProf->prepare_read_env = NULL;
    gloveProf->prepare_write_env = NULL;
    gloveProf->prepare_read_env = pvPortMalloc(sizeof(prepare_type_env_t));
    gloveProf->prepare_write_env = pvPortMalloc(sizeof(prepare_type_env_t));
    gloveProf->prepare_read_env->prepare_buf = NULL;
    gloveProf->prepare_write_env->prepare_buf = NULL;
    return gloveProf;
}

void disableProfileNotifications(gloveProfile_t *_profile)
{
    _profile->cccd.battery = NOTIFICATION_DISABLE;
    _profile->cccd.fb_led = NOTIFICATION_DISABLE;
    _profile->cccd.flex_sensor = NOTIFICATION_DISABLE;
    _profile->cccd.gestures = NOTIFICATION_DISABLE;
    _profile->cccd.restart = NOTIFICATION_DISABLE;
    #ifdef ENABLE_THEMU_BLE_LOGS
    _profile->cccd.ble_log = NOTIFICATION_DISABLE;
    #endif
}

//Verify for write permission and CCCD value. Return the CCCD value on success or 0 on fail.
uint16_t getCCCD(esp_ble_gatts_cb_param_t *_param)
{
    if(_param->write.len == 2)
        {
        uint16_t descr_value = _param->write.value[1]<<8 | _param->write.value[0];
        if (descr_value == 0x0001){
            if (a_property & ESP_GATT_CHAR_PROP_BIT_NOTIFY){
                ESP_LOGI(GATTS_TAG, "Notify enable");
                return NOTIFICATION_ENABLE;
            }
        }else if (descr_value == 0x0002){
            if (a_property & ESP_GATT_CHAR_PROP_BIT_INDICATE){
                ESP_LOGI(GATTS_TAG, "Indicate enable");
                return INDICATIONS_ENABLE;
            }
        }
        else if (descr_value == 0x0000){
            ESP_LOGI(GATTS_TAG, "Notify & indicate disable ");
            return NOTIFICATION_DISABLE;
        }else{
            ESP_LOGE(GATTS_TAG, "Unknown descriptor value.");
        }
    }
    else
    {
        ESP_LOGE(GATTS_TAG, "Descriptor length out of range. Expected 16 bits.");
    } 
    return 0;
}

static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event) {
#ifdef CONFIG_SET_RAW_ADV_DATA
    case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT:
        adv_config_done &= (~adv_config_flag);
        if (adv_config_done==0){
            esp_ble_gap_start_advertising(&adv_params);
        }
        break;
    case ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT:
        adv_config_done &= (~scan_rsp_config_flag);
        if (adv_config_done==0){
            esp_ble_gap_start_advertising(&adv_params);
        }
        break;
#endif
    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
        //advertising start complete event to indicate advertising start successfully or failed
        if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(GATTS_TAG, "Advertising start failed\n");
        }
        break;
    case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
        if (param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(GATTS_TAG, "Advertising stop failed\n");
        } else {
            ESP_LOGI(GATTS_TAG, "Stop adv successfully\n");
        }
        break;
    case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
         ESP_LOGI(GATTS_TAG, "update connection params status = %d, min_int = %d, max_int = %d,conn_int = %d,latency = %d, timeout = %d",
                  param->update_conn_params.status,
                  param->update_conn_params.min_int,
                  param->update_conn_params.max_int,
                  param->update_conn_params.conn_int,
                  param->update_conn_params.latency,
                  param->update_conn_params.timeout);
        break;
    default:
        break;
    }
}

static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    /* If event is register event, store the gatts_if for each profile */
    if (event == ESP_GATTS_REG_EVT) {
        if (param->reg.status == ESP_GATT_OK) {
            gl_profile_tab[param->reg.app_id].gatts_if = gatts_if;
        } else {
            ESP_LOGI(GATTS_TAG, "Reg app failed, app_id %04x, status %d\n",
                    param->reg.app_id,
                    param->reg.status);
            return;
        }
    }

    /* If the gatts_if equal to profile A, call profile A cb handler,
     * so here call each profile's callback */
    do {
        int idx;
        for (idx = 0; idx < PROFILE_NUM; idx++) {
            if (gatts_if == ESP_GATT_IF_NONE || /* ESP_GATT_IF_NONE, not specify a certain gatt_if, need to call every profile cb function */
                    gatts_if == gl_profile_tab[idx].gatts_if) {
                if (gl_profile_tab[idx].gatts_cb) {
                    gl_profile_tab[idx].gatts_cb(event, gatts_if, param);
                }
            }
        }
    } while (0);
}

/*The example_write_event_env() function contains the logic for the write long characteristic procedure:*/
static void example_write_event_env(esp_gatt_if_t gatts_if, prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param){
    esp_gatt_status_t status = ESP_GATT_OK;
    if (param->write.need_rsp){
        if (param->write.is_prep){
            if (prepare_write_env->prepare_buf == NULL) {
                prepare_write_env->prepare_buf = (uint8_t *)pvPortMalloc(PREPARE_BUF_MAX_SIZE*sizeof(uint8_t));
                prepare_write_env->prepare_len = 0;
                if (prepare_write_env->prepare_buf == NULL) {
                    ESP_LOGE(GATTS_TAG, "Gatt_server prep no mem\n");
                    status = ESP_GATT_NO_RESOURCES;
                }
            } else {
                if(param->write.offset > PREPARE_BUF_MAX_SIZE) {
                    status = ESP_GATT_INVALID_OFFSET;
                } else if ((param->write.offset + param->write.len) > PREPARE_BUF_MAX_SIZE) {
                    status = ESP_GATT_INVALID_ATTR_LEN;
                }
            }

            esp_gatt_rsp_t *gatt_rsp = (esp_gatt_rsp_t *)pvPortMalloc(sizeof(esp_gatt_rsp_t));
            gatt_rsp->attr_value.len = param->write.len;
            gatt_rsp->attr_value.handle = param->write.handle;
            gatt_rsp->attr_value.offset = param->write.offset;
            gatt_rsp->attr_value.auth_req = ESP_GATT_AUTH_REQ_NONE;
            memcpy(gatt_rsp->attr_value.value, param->write.value, param->write.len);
            esp_err_t response_err = esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, status, gatt_rsp);
            if (response_err != ESP_OK){
               ESP_LOGE(GATTS_TAG, "Send response error\n");
            }
            vPortFree(gatt_rsp);
            if (status != ESP_GATT_OK){
                return;
            }
            memcpy(prepare_write_env->prepare_buf + param->write.offset,
                   param->write.value,
                   param->write.len);
            prepare_write_env->prepare_len += param->write.len;

        }else{
            esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, status, NULL);
        }
    }
}

static void example_exec_write_event_env(prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param){
    if (param->exec_write.exec_write_flag == ESP_GATT_PREP_WRITE_EXEC){
        esp_log_buffer_hex(GATTS_TAG, prepare_write_env->prepare_buf, prepare_write_env->prepare_len);
    }else{
        ESP_LOGI(GATTS_TAG,"ESP_GATT_PREP_WRITE_CANCEL");
    }
    if (prepare_write_env->prepare_buf) {
        vPortFree(prepare_write_env->prepare_buf);
        prepare_write_env->prepare_buf = NULL;
    }
    prepare_write_env->prepare_len = 0;
}

static void gatts_profile_a_write_handle(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    if (!param->write.is_prep){
        ESP_LOGI(GATTS_TAG, "GATT_WRITE_EVT, value len %d, value :", param->write.len);
        esp_log_buffer_hex(GATTS_TAG, param->write.value, param->write.len);
    }

    
    if(param->write.handle == flex_sensor_descr_handle)
    {
        a->cccd.flex_sensor = getCCCD(param);
    }
    else if(param->write.handle == restart_charvalue_handle)
    {
        
    }
    else if(param->write.handle == restart_descr_handle)
    {
        
    }
    else if(param->write.handle == gestures_descr_handle)
    {
        a->cccd.gestures = getCCCD(param);
    }
    else if(param->write.handle == fb_led_charvalue_handle)
    {
        gpio_set_level(FB_LED_RED_PIN,(~param->write.value[0])&BIT(FB_LED_RED_PLOAD_BIT));
        gpio_set_level(FB_LED_GREEN_PIN,(~param->write.value[0])&BIT(FB_LED_GREEN_PLOAD_BIT));
        gpio_set_level(FB_LED_BLUE_PIN,(~param->write.value[0])&BIT(FB_LED_BLUE_PLOAD_BIT));
    }
    else if(param->write.handle == fb_led_descr_handle)
    {
      
    }
    else if(param->write.handle == battery_descr_handle)
    {
        
    }
    else if(param->write.handle == battery_descr2_handle)
    {
        a->cccd.battery = getCCCD(param);
    }
    #ifdef ENABLE_THEMU_BLE_LOGS
    else if(param->write.handle == log_descr_handle)
    {
        a->cccd.ble_log = getCCCD(param);
    } 
    #endif

    example_write_event_env(gatts_if, a->prepare_write_env, param); 
}

static void gatts_profile_a_read_handle(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    esp_gatt_rsp_t rsp;
    /*This response buffer is managed as a data array instead of a data pointer.
    * So just a memset() is enough to allocate memory.*/
    memset(&rsp, 0, sizeof(esp_gatt_rsp_t));
    rsp.attr_value.handle = param->read.handle;

    /*Supporting long reads (more than the 22 bytes standard payload)*/
    if (param->read.is_long && a->prepare_read_env->prepare_buf)
    {
        int remaining_read = (a->prepare_read_env->prepare_len - param->read.offset);
        rsp.attr_value.len = (remaining_read) > PAYLOAD_LEN ? PAYLOAD_LEN : remaining_read;
        memcpy(rsp.attr_value.value, a->prepare_read_env->prepare_buf + param->read.offset, rsp.attr_value.len);
        if (param->read.offset + rsp.attr_value.len >= a->prepare_read_env->prepare_len) 
        {
            if (a->prepare_read_env->prepare_buf) 
            {
                vPortFree(a->prepare_read_env->prepare_buf);
                a->prepare_read_env->prepare_buf = NULL;
            }
            a->prepare_read_env->prepare_len = 0;
        }
    }else{
        discardPayload(a->prepare_read_env);
        if(param->read.handle == flex_sensor_charvalue_handle)
        {  
            #ifdef ENABLE_THEMU_ADC
            prepReadFlexSensors(a->prepare_read_env);             
            #else
            prepReadDummyBytes(a->prepare_read_env, 5);
            #endif
        }
        else if(param->read.handle == flex_sensor_descr_handle)
        {
            prepReadCCCD(a->prepare_read_env, a->cccd.flex_sensor);
        }
        else if(param->read.handle == restart_charvalue_handle)
        {
            prepReadDummyBytes(a->prepare_read_env, 1);
        }
        else if(param->read.handle == restart_descr_handle)
        {
            prepReadDummyBytes(a->prepare_read_env, 2);
        }
        else if(param->read.handle == gestures_charvalue_handle)
        {
            #ifdef ENABLE_THEMU_GESTURES
            prepReadGestures(a->prepare_read_env, gesture, &gesturesPayload);
            #elif defined(ENABLE_THEMU_TEST_APP)
            uint8_t buf[] = {0x01};
            prepReadCustomBytes(a->prepare_read_env, sizeof(buf), buf);
            #else
            prepReadDummyBytes(a->prepare_read_env, 1);
            #endif
        }
        else if(param->read.handle == gestures_descr_handle)
        {
            prepReadCCCD(a->prepare_read_env, a->cccd.gestures);
        }
        else if(param->read.handle == fb_led_charvalue_handle)
        {
            prepReadFBLed(a->prepare_read_env);
        }
        else if(param->read.handle == fb_led_descr_handle)
        {
            prepReadDummyBytes(a->prepare_read_env, 2);
        }
        else if(param->read.handle == battery_charvalue_handle)
        {
            #ifdef ENABLE_THEMU_ADC
            prepReadBatteryLevel(a->prepare_read_env);
            #else
            prepReadDummyBytes(a->prepare_read_env, 1);
            #endif
        }
        else if(param->read.handle == battery_descr_handle)
        {
            prepReadDummyBytes(a->prepare_read_env, 2);
        }
        else if(param->read.handle == battery_descr2_handle)
        {
            prepReadCCCD(a->prepare_read_env, a->cccd.battery);
        }
        #ifdef ENABLE_THEMU_BLE_LOGS
        else if(param->read.handle == log_descr_handle)
        {
            prepReadCCCD(a->prepare_read_env, a->cccd.ble_log);
        }
        #endif
        else
        {
            prepReadDummyBytes(a->prepare_read_env, 1);
        }
        
        rsp.attr_value.len = a->prepare_read_env->prepare_len;
        memcpy(rsp.attr_value.value, a->prepare_read_env->prepare_buf, rsp.attr_value.len);  
    }
    
    esp_ble_gatts_send_response(gatts_if, param->read.conn_id, param->read.trans_id,
                                ESP_GATT_OK, &rsp);
    discardPayload(a->prepare_read_env);
}

/*Application Profiles callback functions*/
static void gatts_profile_a_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    switch (event) {
    case ESP_GATTS_REG_EVT:
        ESP_LOGI(GATTS_TAG, "REGISTER_APP_EVT, status %d, app_id %d\n", param->reg.status, param->reg.app_id);

        esp_err_t set_dev_name_ret = esp_ble_gap_set_device_name(TEST_DEVICE_NAME); //Used if not advertising cuztomized raw data.
        if (set_dev_name_ret){
            ESP_LOGE(GATTS_TAG, "set device name failed, error code = %x", set_dev_name_ret);
        }
#ifdef CONFIG_SET_RAW_ADV_DATA
        //config adv data
        esp_err_t raw_adv_ret = esp_ble_gap_config_adv_data_raw(raw_adv_data, sizeof(raw_adv_data));
        if (raw_adv_ret){
            ESP_LOGE(GATTS_TAG, "config raw adv data failed, error code = %x ", raw_adv_ret);
        }
        adv_config_done |= adv_config_flag;
        //config scan response data
        esp_err_t raw_scan_ret = esp_ble_gap_config_scan_rsp_data_raw(raw_scan_rsp_data, sizeof(raw_scan_rsp_data));
        if (raw_scan_ret){
            ESP_LOGE(GATTS_TAG, "config raw scan rsp data failed, error code = %x", raw_scan_ret);
        }
        adv_config_done |= scan_rsp_config_flag;
#endif
        /*Creating 5 primary services for profile app A*/
        gl_profile_tab[PROFILE_A_APP_ID].service_id.id.inst_id = 0x00;
        gl_profile_tab[PROFILE_A_APP_ID].service_id.id.uuid.len = ESP_UUID_LEN_16;

        gl_profile_tab[PROFILE_A_APP_ID].service_id.is_primary = true;
        gl_profile_tab[PROFILE_A_APP_ID].service_id.id.uuid.uuid.uuid16 = FLEX_SENSOR_SERVICE_UUID;
        esp_ble_gatts_create_service(gatts_if, &gl_profile_tab[PROFILE_A_APP_ID].service_id, FLEX_SENSOR_NUM_HANDLE); /*Creating a service triggers an ESP_GATTS_CREATE_EVT*/
        gl_profile_tab[PROFILE_A_APP_ID].service_id.is_primary = true;
        gl_profile_tab[PROFILE_A_APP_ID].service_id.id.uuid.uuid.uuid16 = RESTART_SERVICE_UUID;
        esp_ble_gatts_create_service(gatts_if, &gl_profile_tab[PROFILE_A_APP_ID].service_id, RESTART_NUM_HANDLE);
        gl_profile_tab[PROFILE_A_APP_ID].service_id.is_primary = true;
        gl_profile_tab[PROFILE_A_APP_ID].service_id.id.uuid.uuid.uuid16 = GESTURES_SERVICE_UUID;
        esp_ble_gatts_create_service(gatts_if, &gl_profile_tab[PROFILE_A_APP_ID].service_id, GESTURES_NUM_HANDLE);
        gl_profile_tab[PROFILE_A_APP_ID].service_id.is_primary = true;
        gl_profile_tab[PROFILE_A_APP_ID].service_id.id.uuid.uuid.uuid16 = FB_LED_SERVICE_UUID;
        esp_ble_gatts_create_service(gatts_if, &gl_profile_tab[PROFILE_A_APP_ID].service_id, FB_LED_NUM_HANDLE);
        gl_profile_tab[PROFILE_A_APP_ID].service_id.is_primary = true;
        gl_profile_tab[PROFILE_A_APP_ID].service_id.id.uuid.uuid.uuid16 = BATTERY_SERVICE_UUID;
        esp_ble_gatts_create_service(gatts_if, &gl_profile_tab[PROFILE_A_APP_ID].service_id, BATTERY_NUM_HANDLE);
        #ifdef ENABLE_THEMU_BLE_LOGS
        gl_profile_tab[PROFILE_A_APP_ID].service_id.is_primary = true;
        gl_profile_tab[PROFILE_A_APP_ID].service_id.id.uuid.uuid.uuid16 = LOG_SERVICE_UUID;
        esp_ble_gatts_create_service(gatts_if, &gl_profile_tab[PROFILE_A_APP_ID].service_id, LOG_NUM_HANDLE);
        #endif
        
        break;
    case ESP_GATTS_READ_EVT: {
        ESP_LOGI(GATTS_TAG, "GATT_READ_EVT, conn_id %d, trans_id %d, handle %d\n", param->read.conn_id, param->read.trans_id, param->read.handle);
        gatts_profile_a_read_handle(gatts_if, param);
        break;
    }
    case ESP_GATTS_WRITE_EVT: {
        ESP_LOGI(GATTS_TAG, "GATT_WRITE_EVT, conn_id %d, trans_id %d, handle %d", param->write.conn_id, param->write.trans_id, param->write.handle);
        gatts_profile_a_write_handle(gatts_if, param);
        break;
    }
    case ESP_GATTS_EXEC_WRITE_EVT:
        ESP_LOGI(GATTS_TAG,"ESP_GATTS_EXEC_WRITE_EVT");
        esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, ESP_GATT_OK, NULL);
        example_exec_write_event_env(a->prepare_write_env, param);
        break;
    case ESP_GATTS_MTU_EVT:
        ESP_LOGI(GATTS_TAG, "ESP_GATTS_MTU_EVT, MTU %d", param->mtu.mtu);
        break;
    case ESP_GATTS_UNREG_EVT:
        break;
    case ESP_GATTS_CREATE_EVT:
        ESP_LOGI(GATTS_TAG, "CREATE_SERVICE_EVT, status %d,  service_handle %d\n", param->create.status, param->create.service_handle);

        if( param->create.service_handle == flex_sensor_handle)
        {
            gl_profile_tab[PROFILE_A_APP_ID].service_handle = param->create.service_handle;
            gl_profile_tab[PROFILE_A_APP_ID].char_uuid.len = ESP_UUID_LEN_16;
            gl_profile_tab[PROFILE_A_APP_ID].char_uuid.uuid.uuid16 = FLEX_SENSOR_CHAR_UUID;
            gl_profile_tab[PROFILE_A_APP_ID].descr_uuid.len = ESP_UUID_LEN_16;
            gl_profile_tab[PROFILE_A_APP_ID].descr_uuid.uuid.uuid16 = FLEX_SENSOR_DESCR_UUID;
            a_property = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY;
            esp_err_t add_char_ret = esp_ble_gatts_add_char(gl_profile_tab[PROFILE_A_APP_ID].service_handle, &gl_profile_tab[PROFILE_A_APP_ID].char_uuid,
                                                            ESP_GATT_PERM_READ,
                                                            a_property,
                                                            NULL, NULL);/* Adding a characteristic to a service triggers an ESP_GATTS_ADD_CHAR_EVT */
            if (add_char_ret){
                ESP_LOGE(GATTS_TAG, "add char failed, error code =%x",add_char_ret);
            }
            esp_err_t add_descr_ret = esp_ble_gatts_add_char_descr(gl_profile_tab[PROFILE_A_APP_ID].service_handle, &gl_profile_tab[PROFILE_A_APP_ID].descr_uuid,
                                                                    ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, NULL, NULL);
                                                                    /* Once the descriptor is added, the ESP_GATTS_ADD_CHAR_DESCR_EVT event is triggered */
            if (add_descr_ret){
                ESP_LOGE(GATTS_TAG, "add char descr failed, error code =%x", add_descr_ret);
            }
        }
        if( param->create.service_handle == restart_handle)
        {
            gl_profile_tab[PROFILE_A_APP_ID].service_handle = param->create.service_handle;
            gl_profile_tab[PROFILE_A_APP_ID].char_uuid.len = ESP_UUID_LEN_16;
            gl_profile_tab[PROFILE_A_APP_ID].char_uuid.uuid.uuid16 = RESTART_CHAR_UUID;
            gl_profile_tab[PROFILE_A_APP_ID].descr_uuid.len = ESP_UUID_LEN_16;
            gl_profile_tab[PROFILE_A_APP_ID].descr_uuid.uuid.uuid16 = RESTART_DESCR_UUID;
            a_property = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE;
            esp_err_t add_char_ret = esp_ble_gatts_add_char(gl_profile_tab[PROFILE_A_APP_ID].service_handle, &gl_profile_tab[PROFILE_A_APP_ID].char_uuid,
                                                            ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                                                            a_property,
                                                            NULL, NULL);/* Adding a characteristic to a service triggers an ESP_GATTS_ADD_CHAR_EVT */
            if (add_char_ret){
                ESP_LOGE(GATTS_TAG, "add char failed, error code =%x",add_char_ret);
            }
            esp_err_t add_descr_ret = esp_ble_gatts_add_char_descr(gl_profile_tab[PROFILE_A_APP_ID].service_handle, &gl_profile_tab[PROFILE_A_APP_ID].descr_uuid,
                                                                    ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, NULL, NULL);
                                                                    /* Once the descriptor is added, the ESP_GATTS_ADD_CHAR_DESCR_EVT event is triggered */
            if (add_descr_ret){
                ESP_LOGE(GATTS_TAG, "add char descr failed, error code =%x", add_descr_ret);
            }
        }
        if( param->create.service_handle == gestures_handle)
        {
            gl_profile_tab[PROFILE_A_APP_ID].service_handle = param->create.service_handle;
            gl_profile_tab[PROFILE_A_APP_ID].char_uuid.len = ESP_UUID_LEN_16;
            gl_profile_tab[PROFILE_A_APP_ID].char_uuid.uuid.uuid16 = GESTURES_CHAR_UUID;
            gl_profile_tab[PROFILE_A_APP_ID].descr_uuid.len = ESP_UUID_LEN_16;
            gl_profile_tab[PROFILE_A_APP_ID].descr_uuid.uuid.uuid16 = GESTURES_DESCR_UUID;
            a_property = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY;
            esp_err_t add_char_ret = esp_ble_gatts_add_char(gl_profile_tab[PROFILE_A_APP_ID].service_handle, &gl_profile_tab[PROFILE_A_APP_ID].char_uuid,
                                                            ESP_GATT_PERM_READ,
                                                            a_property,
                                                            NULL, NULL);
            if (add_char_ret){
                ESP_LOGE(GATTS_TAG, "add char failed, error code =%x",add_char_ret);
            }
            esp_err_t add_descr_ret = esp_ble_gatts_add_char_descr(gl_profile_tab[PROFILE_A_APP_ID].service_handle, &gl_profile_tab[PROFILE_A_APP_ID].descr_uuid,
                                                                    ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, NULL, NULL);
            if (add_descr_ret){
                ESP_LOGE(GATTS_TAG, "add char descr failed, error code =%x", add_descr_ret);
            }
        }
        if( param->create.service_handle == fb_led_handle)
        {
            gl_profile_tab[PROFILE_A_APP_ID].service_handle = param->create.service_handle;
            gl_profile_tab[PROFILE_A_APP_ID].char_uuid.len = ESP_UUID_LEN_16;
            gl_profile_tab[PROFILE_A_APP_ID].char_uuid.uuid.uuid16 = FB_LED_CHAR_UUID;
            gl_profile_tab[PROFILE_A_APP_ID].descr_uuid.len = ESP_UUID_LEN_16;
            gl_profile_tab[PROFILE_A_APP_ID].descr_uuid.uuid.uuid16 = FB_LED_DESCR_UUID;
            a_property = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE;
            esp_err_t add_char_ret = esp_ble_gatts_add_char(gl_profile_tab[PROFILE_A_APP_ID].service_handle, &gl_profile_tab[PROFILE_A_APP_ID].char_uuid,
                                                            ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                                                            a_property,
                                                            NULL, NULL);
            if (add_char_ret){
                ESP_LOGE(GATTS_TAG, "add char failed, error code =%x",add_char_ret);
            }
            esp_err_t add_descr_ret = esp_ble_gatts_add_char_descr(gl_profile_tab[PROFILE_A_APP_ID].service_handle, &gl_profile_tab[PROFILE_A_APP_ID].descr_uuid,
                                                                    ESP_GATT_PERM_READ, NULL, NULL);
            if (add_descr_ret){
                ESP_LOGE(GATTS_TAG, "add char descr failed, error code =%x", add_descr_ret);
            }
        }
        if( param->create.service_handle == battery_handle)
        {
            gl_profile_tab[PROFILE_A_APP_ID].service_handle = param->create.service_handle;
            gl_profile_tab[PROFILE_A_APP_ID].char_uuid.len = ESP_UUID_LEN_16;
            gl_profile_tab[PROFILE_A_APP_ID].char_uuid.uuid.uuid16 = BATTERY_CHAR_UUID;
            gl_profile_tab[PROFILE_A_APP_ID].descr_uuid.len = ESP_UUID_LEN_16;
            gl_profile_tab[PROFILE_A_APP_ID].descr_uuid.uuid.uuid16 = BATTERY_DESCR_UUID;
            a_property = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_NOTIFY;
            esp_err_t add_char_ret = esp_ble_gatts_add_char(gl_profile_tab[PROFILE_A_APP_ID].service_handle, &gl_profile_tab[PROFILE_A_APP_ID].char_uuid,
                                                            ESP_GATT_PERM_READ,
                                                            a_property,
                                                            NULL, NULL);
            if (add_char_ret){
                ESP_LOGE(GATTS_TAG, "add char failed, error code =%x",add_char_ret);
            }
            esp_err_t add_descr_ret = esp_ble_gatts_add_char_descr(gl_profile_tab[PROFILE_A_APP_ID].service_handle, &gl_profile_tab[PROFILE_A_APP_ID].descr_uuid,
                                                                    ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, NULL, NULL);
            if (add_descr_ret){
                ESP_LOGE(GATTS_TAG, "add char descr failed, error code =%x", add_descr_ret);
            }
            gl_profile_tab[PROFILE_A_APP_ID].descr_uuid.uuid.uuid16 = BATTERY_DESCR2_UUID;
            add_descr_ret = esp_ble_gatts_add_char_descr(gl_profile_tab[PROFILE_A_APP_ID].service_handle, &gl_profile_tab[PROFILE_A_APP_ID].descr_uuid,
                                                                    ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, NULL, NULL);
            if (add_descr_ret){
                ESP_LOGE(GATTS_TAG, "add char descr failed, error code =%x", add_descr_ret);
            }
        }

        #ifdef ENABLE_THEMU_BLE_LOGS
        if( param->create.service_handle == log_handle)
        {
            gl_profile_tab[PROFILE_A_APP_ID].service_handle = param->create.service_handle;
            gl_profile_tab[PROFILE_A_APP_ID].char_uuid.len = ESP_UUID_LEN_16;
            gl_profile_tab[PROFILE_A_APP_ID].char_uuid.uuid.uuid16 = LOG_CHAR_UUID;
            gl_profile_tab[PROFILE_A_APP_ID].descr_uuid.len = ESP_UUID_LEN_16;
            gl_profile_tab[PROFILE_A_APP_ID].descr_uuid.uuid.uuid16 = LOG_DESCR_UUID;
            a_property = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY;
            esp_err_t add_char_ret = esp_ble_gatts_add_char(gl_profile_tab[PROFILE_A_APP_ID].service_handle, &gl_profile_tab[PROFILE_A_APP_ID].char_uuid,
                                                            ESP_GATT_PERM_READ,
                                                            a_property,
                                                            NULL, NULL);
            if (add_char_ret){
                ESP_LOGE(GATTS_TAG, "add char failed, error code =%x",add_char_ret);
            }
            esp_err_t add_descr_ret = esp_ble_gatts_add_char_descr(gl_profile_tab[PROFILE_A_APP_ID].service_handle, &gl_profile_tab[PROFILE_A_APP_ID].descr_uuid,
                                                                    ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, NULL, NULL);
            if (add_descr_ret){
                ESP_LOGE(GATTS_TAG, "add char descr failed, error code =%x", add_descr_ret);
            }
        }
        #endif
        
        esp_ble_gatts_start_service(gl_profile_tab[PROFILE_A_APP_ID].service_handle);/* Starting a service triggers an ESP_GATTS_START_EVT */  
        break;
    case ESP_GATTS_ADD_INCL_SRVC_EVT:
        break;
    case ESP_GATTS_ADD_CHAR_EVT: {
        gl_profile_tab[PROFILE_A_APP_ID].char_handle = param->add_char.attr_handle;
        ESP_LOGI(GATTS_TAG, "ADD_CHAR_EVT, status %d,  attr_handle %d, service_handle %d\n",
                param->add_char.status, param->add_char.attr_handle, param->add_char.service_handle);
        break;
    }
    case ESP_GATTS_ADD_CHAR_DESCR_EVT:
        gl_profile_tab[PROFILE_A_APP_ID].descr_handle = param->add_char_descr.attr_handle;
        ESP_LOGI(GATTS_TAG, "ADD_DESCR_EVT, status %d, attr_handle %d, service_handle %d\n",
                 param->add_char_descr.status, param->add_char_descr.attr_handle, param->add_char_descr.service_handle);
        break;
    case ESP_GATTS_DELETE_EVT:
        break;
    case ESP_GATTS_START_EVT:
        ESP_LOGI(GATTS_TAG, "SERVICE_START_EVT, status %d, service_handle %d\n",
                 param->start.status, param->start.service_handle);
        break;
    case ESP_GATTS_STOP_EVT:
        break;
    case ESP_GATTS_CONNECT_EVT: {
        esp_ble_conn_update_params_t conn_params = {0};
        memcpy(conn_params.bda, param->connect.remote_bda, sizeof(esp_bd_addr_t));
        /* For the IOS system, please reference the apple official documents about the ble connection parameters restrictions. */
        conn_params.latency = 0;
        conn_params.max_int = 0x20;     // max_int = 0x20*1.25ms = 40ms
        conn_params.min_int = 0x10;     // min_int = 0x10*1.25ms = 20ms
        conn_params.timeout = 400;      // timeout = 400*10ms = 4000ms
        ESP_LOGI(GATTS_TAG, "CONNECT_EVT, conn_id %d, remote %02x:%02x:%02x:%02x:%02x:%02x:",
                 param->connect.conn_id,
                 param->connect.remote_bda[0], param->connect.remote_bda[1], param->connect.remote_bda[2],
                 param->connect.remote_bda[3], param->connect.remote_bda[4], param->connect.remote_bda[5]);
        gl_profile_tab[PROFILE_A_APP_ID].conn_id = param->connect.conn_id;
        //start sent the update connection parameters to the peer device.
        esp_ble_gap_update_conn_params(&conn_params); /*this triggers a GAP event ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT*/

        /*Saving app parameters to handle notifications*/
        a->gatts_if = gatts_if;
        a->conn_id = param->connect.conn_id;
        disableProfileNotifications(a);
        timer_start(TIMER_GROUP_0, TIMER_0);
        break;
    }
    case ESP_GATTS_DISCONNECT_EVT:
        ESP_LOGI(GATTS_TAG, "ESP_GATTS_DISCONNECT_EVT, disconnect reason 0x%x", param->disconnect.reason);
        disableProfileNotifications(a);
        timer_pause(TIMER_GROUP_0, TIMER_0);
        esp_ble_gap_start_advertising(&adv_params);
        break;
    case ESP_GATTS_CONF_EVT:
        ESP_LOGI(GATTS_TAG, "ESP_GATTS_CONF_EVT, status %d attr_handle %d", param->conf.status, param->conf.handle);
        if (param->conf.status != ESP_GATT_OK){
            esp_log_buffer_hex(GATTS_TAG, param->conf.value, param->conf.len);
        }
        break;
    case ESP_GATTS_OPEN_EVT:
    case ESP_GATTS_CANCEL_OPEN_EVT:
    case ESP_GATTS_CLOSE_EVT:
    case ESP_GATTS_LISTEN_EVT:
    case ESP_GATTS_CONGEST_EVT:
    default:
        break;
    }
}

void InitBLE()
{
    a = createGloveProfile();
    esp_err_t ret;

    /*Initialize NVS. (Non-Volatile Storage library)
    * This library allows to save key-value pairs in flash memory and is used by some components such as the Wi-Fi library to save the SSID and password:*/
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

    /*The BT controller implements the Host Controller Interface (HCI) on the controller side, the Link Layer (LL) and the Physical Layer (PHY). 
    * The BT Controller is invisible to the user applications and deals with the lower layers of the BLE stack.
    * The controller configuration includes setting the BT controller stack size, priority and HCI baud rate.*/
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(GATTS_TAG, "%s initialize controller failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }
    
    /*ESP_BT_MODE_IDLE for BT not running*/
    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(GATTS_TAG, "%s enable controller failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    /*After the initialization of the BT controller, the Bluedroid stack, which includes the common definitions and APIs
    * for both BT Classic and BLE, is initialized and enabled:*/
    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(GATTS_TAG, "%s init bluetooth failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }
    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(GATTS_TAG, "%s enable bluetooth failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    /*The Bluetooth stack is up and running at this point in the program flow, however 
    * the functionality of the application has not been defined yet. The functionality is defined by reacting to events 
    * such as what happens when another device tries to read or write parameters and establish a connection. 
    * The two main managers of events are the GAP and GATT event handlers. The application needs to register a callback 
    * function for each event handler in order to let the application know which functions are going to handle the GAP and GATT events.
    * The functions gatts_event_handler() and gap_event_handler() handle all the events that are pushed to the application from the BLE stack.*/
    ret = esp_ble_gatts_register_callback(gatts_event_handler);
    if (ret){
        ESP_LOGE(GATTS_TAG, "gatts register error, error code = %x", ret);
        return;
    }
    ret = esp_ble_gap_register_callback(gap_event_handler);
    if (ret){
        ESP_LOGE(GATTS_TAG, "gap register error, error code = %x", ret);
        return;
    }

    /*Finally, the Application Profiles are registered using the Application ID, which is an user-assigned number to identify each profile. 
    * In this way, multiple Application Profiles can run in one server.*/
    ret = esp_ble_gatts_app_register(PROFILE_A_APP_ID); /*Registering an app triggers an ESP_GATTS_REG_EVT */
    if (ret){
        ESP_LOGE(GATTS_TAG, "gatts app register error, error code = %x", ret);
        return;
    }

    esp_err_t local_mtu_ret = esp_ble_gatt_set_local_mtu(500); /*MTU = Maximum Transmission Unit (or effectively maximum packet length)*/
    if (local_mtu_ret){
        ESP_LOGE(GATTS_TAG, "set local  MTU failed, error code = %x", local_mtu_ret);
    }

    return;
}

/**Why MTU agreement is needed in order to set a notification:
 * Thread: https://github.com/espressif/esp-idf/issues/3315
 * For posterity, I reviewed the Bluetooth specification (v4.2 & v5.1), 
 * and found the sections relevant to Notification (4.10), Indication (4.11) 
 * and write long characteristic values (4.9.4). It does appear (as @chegewara states) 
 * that notification & indication of new characteristic values is limited by the current MTU.
 */
void tBLE (void *pv)
{
    disableProfileNotifications(a);
    discardPayload(a->prepare_read_env);
    discardPayload(a->prepare_write_env);
    gpio_set_level(FB_LED_RED_PIN,1);
    gpio_set_level(FB_LED_GREEN_PIN,1);
    gpio_set_level(FB_LED_BLUE_PIN,1);
    uint32_t notifycount = 0;
    while (1)
    {
        notifycount = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if(notifycount == 1)
        {   
            if(a->cccd.flex_sensor)
            {
                #ifdef ENABLE_THEMU_ADC
                prepReadFlexSensors(a->prepare_read_env);
                #else
                prepReadDummyBytes(a->prepare_read_env, 5);
                #endif
                esp_ble_gatts_send_indicate(a->gatts_if, a->conn_id, flex_sensor_charvalue_handle,
                        a->prepare_read_env->prepare_len, a->prepare_read_env->prepare_buf, false);
                discardPayload(a->prepare_read_env);
            }
            if(a->cccd.restart)
            {

            }
            if(a->cccd.gestures)
            {
                #ifdef ENABLE_THEMU_GESTURES
                prepReadGestures(a->prepare_read_env, gesture, &gesturesPayload);
                #elif defined(ENABLE_THEMU_TEST_APP)
                uint8_t buf[] = {0x01};
                prepReadCustomBytes(a->prepare_read_env, sizeof(buf), buf);
                #else
                prepReadDummyBytes(a->prepare_read_env, 1);
                #endif               
                esp_ble_gatts_send_indicate(a->gatts_if, a->conn_id, gestures_charvalue_handle,
                        a->prepare_read_env->prepare_len, a->prepare_read_env->prepare_buf, false);
                discardPayload(a->prepare_read_env);
            }
            if(a->cccd.fb_led)
            {

            }
            if(a->cccd.battery)
            {
                #ifdef ENABLE_THEMU_ADC
                prepReadBatteryLevel(a->prepare_read_env);
                #else 
                prepReadDummyBytes(a->prepare_read_env, 1);
                #endif
                esp_ble_gatts_send_indicate(a->gatts_if, a->conn_id, battery_charvalue_handle,
                        a->prepare_read_env->prepare_len, a->prepare_read_env->prepare_buf, false);
                discardPayload(a->prepare_read_env);
            }
        }
        #ifdef ENABLE_THEMU_BLE_LOGS
        else if(notifycount == 2)
        {
            if(a_cccd.ble_log)
            {
                prepReadBLELog(a->prepare_read_env, bleLogMsg);
                esp_ble_gatts_send_indicate(a_gatts_if, a_conn_id, log_charvalue_handle,
                        a->prepare_read_env->prepare_len, a->prepare_read_env->prepare_buf, false);
                discardPayload(a->prepare_read_env);
            } 
        }
        #endif
        else if(notifycount == 3)//Disable notifications & clean geastures.
        {
            if(a->cccd.flex_sensor)
            {
                uint8_t buf[] = {0x00, 0x00, 0x00, 0x00, 0x00};
                prepReadCustomBytes(a->prepare_read_env, sizeof(buf), buf);            
                esp_ble_gatts_send_indicate(a->gatts_if, a->conn_id, flex_sensor_charvalue_handle,
                        a->prepare_read_env->prepare_len, a->prepare_read_env->prepare_buf, false);
                discardPayload(a->prepare_read_env);
            }
            if(a->cccd.gestures)
            {
                uint8_t buf[] = {0x00};
                prepReadCustomBytes(a->prepare_read_env, sizeof(buf), buf);            
                esp_ble_gatts_send_indicate(a->gatts_if, a->conn_id, gestures_charvalue_handle,
                        a->prepare_read_env->prepare_len, a->prepare_read_env->prepare_buf, false);
                discardPayload(a->prepare_read_env);
            }
            timer_pause(TIMER_GROUP_0, TIMER_1);
        }
        else if(notifycount == 4)//Enable notifications.
        {
            timer_start(TIMER_GROUP_0, TIMER_1); 
        }
        else
        {
            printf("TIMEOUT waiting notification on tBLE\n");
        }

    }
    
}
