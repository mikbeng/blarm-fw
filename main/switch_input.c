/**
 * @file switch_input.c
 * @brief 
 *
 * @author
 * @date 2025-01-22
 */

#include "switch_input.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/******************************************************************************
 * Private Definitions and Types
 ******************************************************************************/
#define DEBOUNCE_TIME_MS 50  // Adjust this value based on your switch characteristics

/******************************************************************************
 * Private Function Declarations
 ******************************************************************************/

/******************************************************************************
 * Private Variables
 ******************************************************************************/
static uint32_t last_press_time = 0;
static bool last_state = false;

/******************************************************************************
 * Private Function Implementations
 ******************************************************************************/

/******************************************************************************
 * Public Function Implementations
 ******************************************************************************/
void switch_input_init(void)
{
    gpio_config_t input_config = {
        .pin_bit_mask = (1ULL << SWITCH_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&input_config);
}

bool switch_input_is_pressed(void)
{
    bool current_state = (gpio_get_level(SWITCH_PIN) == 0);
    uint32_t current_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
    
    // If the state changed
    if (current_state != last_state) {
        // If enough time has passed since last change
        if ((current_time - last_press_time) >= DEBOUNCE_TIME_MS) {
            last_state = current_state;
            last_press_time = current_time;
        }
    }
    
    return last_state;
}
