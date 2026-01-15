#ifndef SENSOR_TASK_H
#define SENSOR_TASK_H

// Обов'язкові інклюди для FreeRTOS типів
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "sht30.h" 

// 1. Визначення структури (щоб main.c її бачив)
typedef struct {
    sht30_reading_t sensor_data;
    uint32_t timestamp;
} sensor_message_t;

// 2. Прототипи задач (щоб main.c міг їх викликати)
void sensor_task(void* pvParameters);
void start_sensor_task(QueueHandle_t queue);

#endif