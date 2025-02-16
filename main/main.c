#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "buzzer.h"
#include "pwr_mgmt.h"
#include "switch_input.h"
#include "accel.h"
#include "main.h"


void led_init(void)
{
    gpio_config_t output_config = {
        .pin_bit_mask = (1ULL << led_pin),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&output_config);
}

void app_main(void)
{
    vTaskDelay(pdMS_TO_TICKS(5000));
    printf("app_main\n");
    pwr_init(); //Make sure to call this first as it checks if we woke up from deep sleep

    //accel_init();

    led_init();

    //Flash the LED fast for 5 seconds

    pwr_enter_deep_sleep(0xAC);
    //buzzer_init();
    
    int level = 0;

    switch_input_init();
    
    switch_event_t event;
    
    while (1) {
        // Wait for switch events
        if (switch_input_get_event(&event, 50)) {
            switch (event) {
                case SWITCH_EVENT_PRESSED:
                    printf("Switch is pressed!\n");
                    printf("Entering deep sleep in 5 seconds\n");
                    //Flash the LED fast for 5 seconds
                    for (int i = 0; i < 10; i++) {
                        gpio_set_level(led_pin, 1);
                        vTaskDelay(pdMS_TO_TICKS(250));
                        gpio_set_level(led_pin, 0);
                        vTaskDelay(pdMS_TO_TICKS(250));
                    }
                    pwr_enter_deep_sleep(0xAC);
                    break;
                case SWITCH_EVENT_RELEASED:
                    printf("Switch is released!\n");
                    break;
            }
        }

        gpio_set_level(led_pin, level);
        level = !level;

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
