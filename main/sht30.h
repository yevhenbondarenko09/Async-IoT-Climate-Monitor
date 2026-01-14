#ifndef SHT_30_H
#define SHT_30_H
#include "esp_err.h"

#define SHT30_I2C_ADDR  0x44
#define SHT30_CMD_MEASURE 0x2400

typedef struct {
    float temp;      // Температура в градусах Цельсія
    float humidity;  // Вологість у відсотках
} sht30_reading_t;

esp_err_t sht30_init(void);
esp_err_t sht30_read_data(sht30_reading_t *reading);

#endif