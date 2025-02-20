#include "buzzer.h"
#include "driver/ledc.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <math.h>

static TaskHandle_t buzzer_task_handle = NULL;

void buzzer_set_frequency(uint32_t freq_hz) {
    ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, freq_hz);
}

void buzzer_on(uint32_t duty, uint32_t freq_hz) {
    buzzer_set_frequency(freq_hz);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}

void buzzer_off(void) {
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}

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
    
    while (1) {
        printf("Setting buzzer to 5200 Hz\n");
        buzzer_on(4096, 5200);
        vTaskDelay(pdMS_TO_TICKS(2000));
        buzzer_off();
        vTaskDelay(pdMS_TO_TICKS(2000));
        printf("Setting buzzer to 3500 Hz\n");
        buzzer_on(4096, 3500);
        vTaskDelay(pdMS_TO_TICKS(2000));
        buzzer_off();
        vTaskDelay(pdMS_TO_TICKS(2000));
        printf("Setting buzzer to 1800 Hz\n");
        buzzer_on(4096, 1800);
        vTaskDelay(pdMS_TO_TICKS(2000));
        buzzer_off();
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void buzzer_start(void) {
    if (buzzer_task_handle == NULL) {
        xTaskCreate(buzzer_task, "buzzer_task", 2048, NULL, 5, &buzzer_task_handle);
    }
}

void buzzer_stop(void) {
    if (buzzer_task_handle != NULL) {
        vTaskDelete(buzzer_task_handle);
        buzzer_task_handle = NULL;
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    }
}
