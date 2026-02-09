#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

#define LED_GPIO GPIO_NUM_7
#define BUTTON_GPIO GPIO_NUM_13 
#define ADC_CHANNEL ADC1_CHANNEL_4   // (this is GPIO5 on ESP32-S3)
#define ADC_THRESHOLD  1100   // mV
#define DEFAULT_VREF   1100


static const char *TAG = "LED_ADC";

void app_main(void)
{
    // Configuration for the LED (Output)
    gpio_config_t led_io_conf = {};
    led_io_conf.intr_type = GPIO_INTR_DISABLE;      // Disable interrupts for the LED
    led_io_conf.mode = GPIO_MODE_OUTPUT;            // Set pin as output
    led_io_conf.pin_bit_mask = (1ULL << LED_GPIO); // Set the specific LED pin
    led_io_conf.pull_down_en = 0;                   // Disable internal pull-down
    led_io_conf.pull_up_en = 0;                     // Disable internal pull-up
    gpio_config(&led_io_conf);                      // Apply the configuration

        // Configuration for the Button (Input)
    gpio_config_t btn_io_conf = {};
    btn_io_conf.intr_type = GPIO_INTR_DISABLE;      // Disable interrupts for the button
    btn_io_conf.mode = GPIO_MODE_INPUT;             // Set pin as input
    btn_io_conf.pin_bit_mask = (1ULL << BUTTON_GPIO); // Set the specific button pin
    btn_io_conf.pull_up_en = 1;                     // Enable internal pull-up resistor
    btn_io_conf.pull_down_en = 0;                   // Disable internal pull-down
    gpio_config(&btn_io_conf);                      // Apply the configuration

    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN_DB_11); // GPIO5

    esp_adc_cal_characteristics_t adc_chars;
        esp_adc_cal_characterize(
        ADC_UNIT_1,
        ADC_ATTEN_DB_11,
        ADC_WIDTH_BIT_12,
        DEFAULT_VREF,
        &adc_chars
    );

     // --- State Variables ---
    // We'll use these to keep track of the LED and button states.
    int led_state = 0;              // 0 = LED OFF, 1 = LED ON
    int last_button_state = 1;      // 1 = NOT PRESSED (due to pull-up resistor)

    printf("Button-controlled LED starting...\n");

    // --- Main Loop ---
    while (1) {
        // Read the current state of the button
        int current_button_state = gpio_get_level(BUTTON_GPIO);
        // Check for a state change: from NOT PRESSED (1) to PRESSED (0)
        // This is a simple form of debouncing to detect a single press.
        if (current_button_state == 0 && last_button_state == 1) {
            printf("Button pressed! Toggling LED...\n");
            
            // Toggle the LED state
            led_state = !led_state;
            gpio_set_level(LED_GPIO, led_state);

            if(led_state == 1){
                ESP_LOGI(TAG, "LED ON, %d", led_state);
            }
            else{
                ESP_LOGI(TAG, "LED OFF, %d", led_state);
            }
            vTaskDelay(100);

            // Read the ADC value and convert it to voltage to check if the LED is connected
            int raw = adc1_get_raw(ADC1_CHANNEL_4);
            uint32_t voltage = esp_adc_cal_raw_to_voltage(raw, &adc_chars);
            // ESP_LOGI(TAG,"ADC Raw: %d, Voltage: %dmV", raw, voltage);
             
            if(led_state == 1){
                if(voltage < ADC_THRESHOLD){
                    ESP_LOGW(TAG, "LED is not connected Voltage below threshold! %dmV < %dmV", voltage, ADC_THRESHOLD);
                }
                else{
                    ESP_LOGI(TAG, "LED is connected Voltage above threshold! %dmV >= %dmV", voltage, ADC_THRESHOLD);
                }
            }
            vTaskDelay(100);
        }

        // Update the last button state for the next loop iteration
        last_button_state = current_button_state;
        // A small delay to keep the task from consuming too much CPU
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

