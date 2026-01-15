#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h" 
#include "sensor_task.h" 
#include "wifi_app.h"   
#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "mqtt_client.h"
#include "mqtt_app.h"
#include "led_effects.h"

static const char *TAG = "MQTT_APP";
static esp_mqtt_client_handle_t client = NULL; // Глобальний дескриптор клієнта


static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = event_data;
    
    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT Підключено до брокера!");
            led_set_mode(LED_MODE_BREATHING);
            break;
            
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGW(TAG, "MQTT Відключено. Клієнт спробує перепідключитися сам.");
            led_set_mode(LED_MODE_BLINKING);
            break;

        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG, "MQTT Помилка");
            led_set_mode(LED_MODE_BLINKING);
            break;
            
        default:
            break;
    }
}


void mqtt_app_start(void) {
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_BROKER_URI,
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    
    esp_mqtt_client_start(client);
}


void mqtt_send_data(const char *data) {
    if (client == NULL) return;

     int msg_id = esp_mqtt_client_publish(client, MQTT_TOPIC_DATA, data, 0, 0, 0);
    
    if (msg_id != -1) {
        ESP_LOGI(TAG, "Дані відправлено");
        led_set_mode(LED_MODE_BREATHING);
    } else {
        ESP_LOGE(TAG, "Помилка відправки!");
        led_set_mode(LED_MODE_BLINKING);
    }
}

void network_mqtt_task(void *pvParameters) {
    QueueHandle_t queue = (QueueHandle_t)pvParameters;
    sensor_message_t msg;
    char json_buffer[128]; // Буфер для тексту JSON

    ESP_LOGI(TAG, "Network Task запущено. Чекаємо на Wi-Fi...");

    xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT, 
                        pdFALSE, pdTRUE, portMAX_DELAY);

    ESP_LOGI(TAG, "Wi-Fi є! Починаємо обробку черги.");

    while (1) {
        
        if (xQueueReceive(queue, &msg, portMAX_DELAY) == pdPASS) {
             float seconds = (float)msg.timestamp / configTICK_RATE_HZ;
            

            snprintf(json_buffer, sizeof(json_buffer), 
            "{\"temp\": %.2f, \"hum\": %.2f, \"time\": %.2f}",
            msg.sensor_data.temp,
            msg.sensor_data.humidity,
            seconds);    

            mqtt_send_data(json_buffer);
        }
    }
}

void start_network_mqtt_task(QueueHandle_t queue){
    xTaskCreate(network_mqtt_task, "NetworkTask", 4096, (void*)queue, 3, NULL);
} 