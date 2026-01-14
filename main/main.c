#include "freertos/FreeRTOS.h"
#include <stdio.h>
#include "freertos/queue.h"
#include "esp_log.h"
#include "freertos/task.h"
#include "led_effects.h"
#include "app_tasks.h"
#include "sht30.h"

static const char *TAG = "MAIN_APP";
QueueHandle_t sensor_queue = NULL;

void app_main(void)
{
    led_init(); 
    led_start_breathing();
    if (sht30_init() != ESP_OK) {
        ESP_LOGE(TAG, "Помилка ініціалізації SHT30!");
        led_set_mode(LED_MODE_BLINKING);
    }
    else {
        ESP_LOGI(TAG, "SHT30 ініціалізовано успішно");
        led_set_mode(LED_MODE_BREATHING); 
    }
   sensor_queue = xQueueCreate(5, sizeof(sensor_message_t));
    if (sensor_queue == NULL) {
        ESP_LOGE(TAG, "Помилка створення черги!");
        led_set_mode(LED_MODE_BLINKING);
    }
    else{start_sensor_task(sensor_queue);}
 }

