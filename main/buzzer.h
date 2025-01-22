#ifndef BUZZER_H
#define BUZZER_H

#include "driver/gpio.h"

#define buzzer_pin 20

void buzzer_init(void);
void buzzer_on(void);
void buzzer_off(void);

#endif