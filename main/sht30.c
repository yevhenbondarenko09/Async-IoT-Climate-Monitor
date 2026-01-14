#include "sht30.h"
#include "driver/i2c.h"  
#include "esp_log.h"     
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define I2C_MASTER_SDA_IO 23
#define I2C_MASTER_SCL_IO 22
#define I2C_MASTER_FREQ_HZ 100000 
#define I2C_NUM_0 0 

static const char *TAG = "SHT30";

esp_err_t sht30_init(void){
i2c_config_t conf = {
    .mode = I2C_MODE_MASTER,
    .sda_io_num = I2C_MASTER_SDA_IO,         // select SDA GPIO specific to your project
    .sda_pullup_en = GPIO_PULLUP_ENABLE,
    .scl_io_num = I2C_MASTER_SCL_IO,         // select SCL GPIO specific to your project
    .scl_pullup_en = GPIO_PULLUP_ENABLE,
    .master.clk_speed = I2C_MASTER_FREQ_HZ,  // select frequency specific to your project
    .clk_flags = 0,                          // optional; you can use I2C_SCLK_SRC_FLAG_* flags to choose i2c source clock here
};
esp_err_t err = i2c_param_config(I2C_NUM_0, &conf);
    if (err != ESP_OK) return err;
    return i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0);
}

esp_err_t sht30_read_data(sht30_reading_t *reading){
    i2c_cmd_handle_t cmd_handle=i2c_cmd_link_create();
    i2c_master_start(cmd_handle);
    i2c_master_write_byte(cmd_handle, (SHT30_I2C_ADDR<<1)|I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd_handle, (SHT30_CMD_MEASURE >> 8), true);
    i2c_master_write_byte(cmd_handle, (SHT30_CMD_MEASURE & 0xFF), true);
    i2c_master_stop(cmd_handle);
    esp_err_t err = i2c_master_cmd_begin(I2C_NUM_0, cmd_handle, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd_handle); 
    if (err != ESP_OK) return err;

    vTaskDelay(pdMS_TO_TICKS(20));
    uint8_t data[6];
    
    cmd_handle=i2c_cmd_link_create();
    i2c_master_start(cmd_handle);
    i2c_master_write_byte(cmd_handle, (SHT30_I2C_ADDR<<1)|I2C_MASTER_READ, true);
    i2c_master_read(cmd_handle, data, 6, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd_handle);
    err = i2c_master_cmd_begin(I2C_NUM_0, cmd_handle, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd_handle); 
    if (err != ESP_OK) return err;

    uint16_t raw_temp = (data[0] << 8) | data[1];
    reading->temp=-45+175*(raw_temp/65535.0f);

    uint16_t raw_hum = (data[3] << 8) | data[4];
    reading->humidity=100*(raw_hum/65535.0f);
    return ESP_OK;
}

