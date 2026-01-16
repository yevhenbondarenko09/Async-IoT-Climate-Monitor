#ifndef WIFI_APP_H
#define WIFI_APP_H

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_err.h"

#define ESP_WIFI_SSID      "Your_SSID"
#define ESP_WIFI_PASS      "Your_PASSWORD"
#define ESP_MAXIMUM_RETRY  5


#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

extern EventGroupHandle_t s_wifi_event_group;

void wifi_init_sta(void);

#endif