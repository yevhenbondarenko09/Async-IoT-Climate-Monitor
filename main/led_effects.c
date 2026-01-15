#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led_effects.h"
#include "driver/ledc.h"

#define LEDC_DUTY_MAX 1023
#define LEDC_DUTY_MIN 0

static const char *TAG = "led_task";
static led_mode_t current_mode = LED_MODE_BREATHING;

esp_err_t led_init(void){

    ledc_timer_config_t timer_conf={
.freq_hz=LEDC_FREQ_HZ,
.duty_resolution=LEDC_RESOLUTION,
.timer_num=LEDC_TIMER,
.speed_mode=LEDC_LOW_SPEED_MODE,
.clk_cfg=LEDC_AUTO_CLK       
};

esp_err_t err= ledc_timer_config(&timer_conf);
if(err!=ESP_OK) return err;

ledc_channel_config_t channel_conf={
.gpio_num=LED_GPIO,
.channel=LEDC_CHANNEL,
.hpoint=0,
.duty=0,
.timer_sel=LEDC_TIMER,
.speed_mode=LEDC_LOW_SPEED_MODE
};

err=ledc_channel_config(&channel_conf);
if(err!=ESP_OK) return err;
return ESP_OK;
}

void led_set_mode(led_mode_t mode) {
    current_mode = mode;
}

static void led_breathing_task(void *pvParameters){
    int duty = 0;
    int step = 5;
    int blink_counter = 0;
    while(1){
  if (current_mode == LED_MODE_BREATHING) {
    duty+=step;
        if (duty >= LEDC_DUTY_MAX) {duty = LEDC_DUTY_MAX; step = -5;} 
        else if (duty <= LEDC_DUTY_MIN) {duty = LEDC_DUTY_MIN; step = 5;}}
 else if (current_mode == LED_MODE_BLINKING) {
            blink_counter++;
            if (blink_counter >= 10) { // Кожні 10 циклів (150 мс)
                duty = (duty == 0) ? LEDC_DUTY_MAX : LEDC_DUTY_MIN; // Перемикаємо 0 або 1023
                blink_counter = 0;
            }
        }

    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL);
    vTaskDelay(pdMS_TO_TICKS(15));
    }
}

void led_start_breathing(void){
xTaskCreate(led_breathing_task, TAG, 2048, NULL, 5, NULL);
}