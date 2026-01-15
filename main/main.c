#include "freertos/FreeRTOS.h"
#include <stdio.h>
#include "freertos/queue.h"
#include "esp_log.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#include "led_effects.h"
#include "sensor_task.h"
#include "sht30.h"
#include "wifi_app.h"
#include "mqtt_app.h"

static const char *TAG = "MAIN_APP";
QueueHandle_t sensor_queue = NULL;

void app_main(void)
{

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

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

    ESP_LOGI(TAG, "Запуск Wi-Fi...");
    wifi_init_sta();

    sensor_queue = xQueueCreate(5, sizeof(sensor_message_t));
    if (sensor_queue == NULL) {
        ESP_LOGE(TAG, "Помилка створення черги!");
        led_set_mode(LED_MODE_BLINKING);
    }
    else{
        start_sensor_task(sensor_queue);
        mqtt_app_start();
        start_network_mqtt_task(sensor_queue);
    }
 }

