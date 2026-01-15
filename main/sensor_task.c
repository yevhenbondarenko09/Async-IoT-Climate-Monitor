#include "sensor_task.h"
#include "esp_log.h"
#include "led_effects.h"

static const char *TAG = "SENSOR_TASK";

void sensor_task (void* pvParameters) {
    sht30_reading_t sensor_data;
    esp_err_t err;
    sensor_message_t sensor_message;
    QueueHandle_t sensor_queue = (QueueHandle_t)pvParameters;
    if (sensor_queue == NULL) {
        ESP_LOGE(TAG, "Queue handle is NULL! Task deleted.");
        vTaskDelete(NULL);
        return;
    }
    while(1) {
        err = sht30_read_data(&sensor_data);

        if(err == ESP_OK) {
            led_set_mode(LED_MODE_BREATHING); 
            sensor_message.sensor_data = sensor_data;
            sensor_message.timestamp = xTaskGetTickCount();
            float seconds = (float)sensor_message.timestamp / configTICK_RATE_HZ;
            ESP_LOGI(TAG, "Температура: %.2f, Вологість: %.2f %%, Час: %.2f c", 
                     sensor_data.temp, sensor_data.humidity, seconds); 

            if (xQueueSend(sensor_queue, &sensor_message, pdMS_TO_TICKS(10)) != pdPASS) {
                ESP_LOGW(TAG, "Черга переповнена!");
            }
        } 
        else {
            ESP_LOGE(TAG, "Не вдалося зчитати дані! Код: %d", err);
            led_set_mode(LED_MODE_BLINKING);
        }

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
    

    vTaskDelete(NULL); 
}
void start_sensor_task(QueueHandle_t queue){
xTaskCreate(sensor_task, "SensorTask", 4096, (void*)queue, 2, NULL);
}
