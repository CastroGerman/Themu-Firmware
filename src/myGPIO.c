/**Brief:
 * GPIO 16 defined as input.
 * GPIO 17 defined as output.
 * 
 */


#include "myGPIO.h"
#include "myBLE.h"
#include "myTasks.h"
#include <string.h>

static esp_adc_cal_characteristics_t *adc_chars;
static const adc_channel_t channel = ADC_CHANNEL_6;     //GPIO34 of ADC1 
static const adc_atten_t atten = ADC_ATTEN_DB_0;
static const adc_unit_t unit = ADC_UNIT_1;

static void check_efuse()
{
    //Check TP is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
        printf("eFuse Two Point: Supported\n");
    } else {
        printf("eFuse Two Point: NOT supported\n");
    }

    //Check Vref is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK) {
        printf("eFuse Vref: Supported\n");
    } else {
        printf("eFuse Vref: NOT supported\n");
    }
}

static void print_char_val_type(esp_adc_cal_value_t val_type)
{
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        printf("Characterized using Two Point Value\n");
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        printf("Characterized using eFuse Vref\n");
    } else {
        printf("Characterized using Default Vref\n");
    }
}

void InitADC1 (void)
{
    //Check if Two Point or Vref are burned into eFuse
    check_efuse();

    //Configure ADC
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(channel, atten);

    //Characterize ADC
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);
    print_char_val_type(val_type);
}

void InitGPIO (void)
{
    gpio_config_t io_config;

    //disable interrupt
    io_config.intr_type = GPIO_PIN_INTR_DISABLE;
    //set as output mode
    io_config.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_config.pin_bit_mask = GPIO_SEL_17;
    //disable pull-down mode
    io_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    //disable pull-up mode
    io_config.pull_up_en = GPIO_PULLUP_DISABLE;
    //configure GPIO with the given settings
    gpio_config(&io_config);

    //interrupt of falling edge
    io_config.intr_type = GPIO_PIN_INTR_NEGEDGE;
    //bit mask of the pins
    io_config.pin_bit_mask = GPIO_SEL_16;
    //set as input mode
    io_config.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_config.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_config);
    //change gpio intrrupt type for one pin
    //gpio_set_intr_type(GPIO_SEL_16, GPIO_INTR_ANYEDGE);
    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_EDGE);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_NUM_16, gpio16_isr_handler, (void*) NULL);
    

    //Setting LED on board
    io_config.intr_type = GPIO_PIN_INTR_DISABLE;
    io_config.mode = GPIO_MODE_INPUT_OUTPUT;
    io_config.pin_bit_mask = GPIO_SEL_2;
    io_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_config.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_config);

}

void readADC1 (void)
{
    uint32_t adc_reading = 0;
    //Multisampling
    for (int i = 0; i < NO_OF_SAMPLES; i++) {
        adc_reading += adc1_get_raw((adc1_channel_t)channel);
    }
    adc_reading /= NO_OF_SAMPLES;
    //Convert adc_reading to voltage in mV
    //uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
    a_prepare_read_env.prepare_buf = pvPortMalloc(sizeof(adc_reading));
    a_prepare_read_env.prepare_len = sizeof(adc_reading);
    memcpy(a_prepare_read_env.prepare_buf, &adc_reading, a_prepare_read_env.prepare_len);
}

void IRAM_ATTR gpio16_isr_handler (void *pv)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xTaskNotifyFromISR(thGPIO, 2, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
    if(xHigherPriorityTaskWoken != pdFALSE){}
}

void tGPIO (void *pv)
{
    uint32_t notifycount = 0;
    while (1)
    {
        notifycount = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);//pdTRUE = as a binary semaphore. pdFALSE = as a counting semaphore.
        if(notifycount == 1)
        {
            
            //printf("Interrumpio notify 1 GPIO 2 \n"); 
            /*Remember that u can't read OUTPUTS, only INPUTS.
            * Or set the GPIO mode to GPIO_MODE_INPUT_OUTPUT.*/
            /*if(gpio_get_level(GPIO_NUM_2))
            {
                gpio_set_level(GPIO_NUM_18, 0);
            }
            else
            {
                gpio_set_level(GPIO_NUM_18, 1);
            }   */        
        }
        else if (notifycount == 2)
        {
            /*
            uint32_t adc_reading = 0;
            //Multisampling
            for (int i = 0; i < NO_OF_SAMPLES; i++) {
                adc_reading += adc1_get_raw((adc1_channel_t)channel);
            }
            adc_reading /= NO_OF_SAMPLES;
            //Convert adc_reading to voltage in mV
            uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
            printf("Raw: %d\tVoltage: %dmV\n", adc_reading, voltage);
            */
            //printf("Interrumpio notify 2 GPIO 2 \n"); 
        }
        else
        {
            printf("TIMEOUT esperando notificacion en tGPIO\n");
        }
    }
}