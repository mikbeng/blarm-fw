#include "buzzer.h"
#include "driver/ledc.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <math.h>

static TaskHandle_t buzzer_task_handle = NULL;

void buzzer_init(void) {
    // Configure LEDC timer
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_13_BIT,    // Set duty resolution to 13 bits
        .freq_hz = 5200,                         
        .speed_mode = LEDC_LOW_SPEED_MODE ,      // High speed mode
        .timer_num = LEDC_TIMER_0                // Use timer 0
    };
    ledc_timer_config(&ledc_timer);

    // Configure LEDC channel
    ledc_channel_config_t ledc_channel = {
        .channel = LEDC_CHANNEL_0,
        .duty = 0,                               // Initial duty cycle 0%
        .gpio_num = buzzer_pin,                  // Your buzzer pin (define this in buzzer.h)
        .speed_mode = LEDC_LOW_SPEED_MODE ,
        .timer_sel = LEDC_TIMER_0
    };
    ledc_channel_config(&ledc_channel);
}

void buzzer_task(void *pvParameters) {
    const float frequency = 2.0f;  // 2 Hz
    const float pi = 3.14159f;
    float time = 0.0f;
    uint32_t duty = 0;
    
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 4096);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);

    while (1) {
        // Calculate sine wave value (-1 to 1)
        //float sine_value = sinf(2 * pi * frequency * time);
        // Convert to duty cycle (0 to 8192, which is 2^13)
        //uint32_t duty = (uint32_t)((sine_value + 1.0f) * 4096.0f);
        
        // duty = 4096;
        // ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
        // ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
        
        // // Increment time (50ms delay = 20 updates per second)
        // vTaskDelay(pdMS_TO_TICKS(500));

        // duty = 4096/2;
        // ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
        // ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
        // //time += 0.05f;  // 50ms in seconds

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void buzzer_on(void) {
    if (buzzer_task_handle == NULL) {
        xTaskCreate(buzzer_task, "buzzer_task", 2048, NULL, 5, &buzzer_task_handle);
    }
}

void buzzer_off(void) {
    if (buzzer_task_handle != NULL) {
        vTaskDelete(buzzer_task_handle);
        buzzer_task_handle = NULL;
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    }
}
