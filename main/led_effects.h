#ifndef LED_EFFECTS_H
#define LED_EFFECTS_H
#include "esp_err.h"

#define LED_GPIO 2
#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_CHANNEL LEDC_CHANNEL_0
#define LEDC_RESOLUTION LEDC_TIMER_10_BIT
#define LEDC_FREQ_HZ         5000

typedef enum {
    LED_MODE_BREATHING, // Нормальний стан
    LED_MODE_BLINKING,  // Помилка (швидке миготіння)
} led_mode_t;

// Функція для зміни режиму
void led_set_mode(led_mode_t mode);
esp_err_t led_init(void);
void led_start_breathing(void);

#endif