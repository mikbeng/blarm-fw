#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "buzzer.h"
#include "pwr_mgmt.h"
#include "switch_input.h"

#define led_pin 8

void gpio_init(void)
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

    gpio_init();
    buzzer_init();
    
    int level = 0;

    while (1) {
        gpio_set_level(led_pin, level);
        level = !level;

        //Read the switch input
        if (switch_input_is_pressed()) {
            printf("Switch is pressed\n");
            vTaskDelay(pdMS_TO_TICKS(3000));
            pwr_enter_deep_sleep(0xAC);
            //buzzer_on();
        }
        else {
            //buzzer_off();
        }

        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}
