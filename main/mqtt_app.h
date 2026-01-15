#ifndef MQTT_APP_H
#define MQTT_APP_H

#define MQTT_BROKER_URI "mqtt://broker.hivemq.com"

#define MQTT_TOPIC_DATA "ukraine/lviv/Bondarenko/ESP32"

void mqtt_app_start(void);
void mqtt_send_data(const char *data);

void network_mqtt_task(void *pvParameters);
void start_network_mqtt_task(QueueHandle_t queue);

#endif
