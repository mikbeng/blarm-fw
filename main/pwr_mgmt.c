/**
 * @file pwr_mgmt.c
 * @brief 
 *
 * @author
 * @date 2025-01-22
 */

#include "pwr_mgmt.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "esp_sleep.h"
#include "esp_log.h"
#include "driver/rtc_cntl.h"
#include "driver/uart.h"
#include "main.h"
#include "driver/adc.h"
#include "soc/periph_defs.h"

/******************************************************************************
 * Private Definitions and Types
 ******************************************************************************/
#define WAKEUP_PIN GPIO_NUM_5
static const char *TAG = "pwr_mgmt";

// RTC memory stored variables
RTC_DATA_ATTR static struct {
    uint32_t wake_count;
    uint64_t last_wakeup_pins;    // Store which pins triggered the wakeup
    uint64_t sleep_time_us;
    uint32_t last_state;          // Can store any state you want to preserve
} sleep_state = {0};

/******************************************************************************
 * Private Function Declarations
 ******************************************************************************/
static void _configure_wakeup_source(void);

/******************************************************************************
 * Private Function Implementations
 ******************************************************************************/
static void _configure_wakeup_source(void)
{
    // Create bitmask for GPIO5
    const uint64_t pin_mask = (1ULL << WAKEUP_PIN);
    
    // Configure the wake-up pin
    gpio_config_t config = {
        .pin_bit_mask = pin_mask,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&config);

    // Enable GPIO wake-up source
    ESP_ERROR_CHECK(esp_deep_sleep_enable_gpio_wakeup(pin_mask, ESP_GPIO_WAKEUP_GPIO_LOW));

}

/******************************************************************************
 * Public Function Implementations
 ******************************************************************************/
void pwr_off(void)
{
    gpio_set_level(pwr_off_pin, 1);
}

void pwr_init(void)
{
    gpio_config_t output_config = {
        .pin_bit_mask = (1ULL << pwr_off_pin),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&output_config);
    //set pwr_off_pin to low
    gpio_set_level(pwr_off_pin, 0);

    gpio_config_t input_config = {
        .pin_bit_mask = (1ULL << bat_mon_pin),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&input_config);
    //set bat_mon_pin to low
    gpio_set_level(bat_mon_pin, 0);

    // Check if we woke up from deep sleep
    esp_sleep_wakeup_cause_t wakeup_cause = esp_sleep_get_wakeup_cause();
    if (wakeup_cause == ESP_SLEEP_WAKEUP_GPIO) { 
        sleep_state.wake_count++;
        // Get and store which pins triggered the wakeup
        sleep_state.last_wakeup_pins = esp_sleep_get_gpio_wakeup_status();
        
        ESP_LOGI(TAG, "Woke up from deep sleep!");
        ESP_LOGI(TAG, "Wake count: %ld", sleep_state.wake_count);
        
        if (sleep_state.last_wakeup_pins & (1ULL << WAKEUP_PIN)) {
            ESP_LOGI(TAG, "Wakeup triggered by button on GPIO %d", WAKEUP_PIN);
        }
        
        // Removed rtc_gpio_deinit as it's not needed for ESP32-C3
        
        // You can access your preserved state here
        ESP_LOGI(TAG, "Last state before sleep: 0x%ld", sleep_state.last_state);
    } else if (wakeup_cause == ESP_SLEEP_WAKEUP_UNDEFINED) {
        ESP_LOGI(TAG, "First boot");
        // Initialize RTC memory variables if needed
        sleep_state.wake_count = 0;
        sleep_state.last_wakeup_pins = 0;
        sleep_state.last_state = 0;
    }
}

void pwr_enter_deep_sleep(uint32_t state_to_save)
{
    
    // Configure ALL GPIO pins as inputs without pullups/pulldowns to minimize current
    for (int i = 2; i <= 7; i++) {  //GPIO2-7 (D0-D5)
        gpio_config_t config = {
            .pin_bit_mask = (1ULL << i),
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE
        };
        gpio_config(&config);
    }

    //GPIO20 (RX_D7) and GPIO21 (TX_D6)
    gpio_config_t config = {
        .pin_bit_mask = (1ULL << 20) | (1ULL << 21),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&config);

    //GPIO8-10 (D8-D10)
    for (int i = 8; i <= 10; i++) {
        gpio_config_t config = {
            .pin_bit_mask = (1ULL << i),
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE
        };
        gpio_config(&config);
    }
    
    // Configure GPIO wakeup
    _configure_wakeup_source();
    
    // Save state and enter deep sleep
    sleep_state.last_state = state_to_save;
    sleep_state.wake_count++;
    
    ESP_LOGI(TAG, "Entering deep sleep");
    vTaskDelay(pdMS_TO_TICKS(100));
    esp_deep_sleep_start();
}

// Helper functions to access RTC stored data
uint32_t pwr_get_wake_count(void)
{
    return sleep_state.wake_count;
}

uint32_t pwr_get_last_state(void)
{
    return sleep_state.last_state;
}

uint64_t pwr_get_last_wakeup_pins(void)
{
    return sleep_state.last_wakeup_pins;
}


