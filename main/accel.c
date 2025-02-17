#include "accel.h"
#include "driver/i2c.h"
#include "esp_log.h"

#define I2C_MASTER_SCL_IO           7        // GPIO for SCL
#define I2C_MASTER_SDA_IO           6        // GPIO for SDA
#define I2C_MASTER_NUM              0        // I2C master number
#define I2C_MASTER_FREQ_HZ          400000   // I2C master clock frequency
#define I2C_TIMEOUT_MS              1000     // I2C timeout in milliseconds

static const char *TAG = "LIS2DH12";

esp_err_t accel_init(void) {
    // Configure I2C
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_DISABLE,
        .scl_pullup_en = GPIO_PULLUP_DISABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ
    };

    esp_err_t ret = i2c_param_config(I2C_MASTER_NUM, &conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C parameter configuration failed");
        return ret;
    }

    ret = i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C driver installation failed");
        return ret;
    }

    // // Verify device ID
    // uint8_t who_am_i;
    // ret = accel_read_reg(LIS2DH12_WHO_AM_I, &who_am_i);
    // if (ret != ESP_OK) {
    //     ESP_LOGE(TAG, "Failed to read WHO_AM_I register");
    //     return ret;
    // }

    // if (who_am_i != 0x33) { // 0x33 is the expected WHO_AM_I value for LIS2DH12
    //     ESP_LOGE(TAG, "Unknown device ID: 0x%02x", who_am_i);
    //     return ESP_ERR_NOT_FOUND;
    // }

    // // CTRL_REG0: Disabel internal pullup on SA0 pin
    // ret = accel_write_reg(LIS2DH12_CTRL_REG0, 0x90);
    // if (ret != ESP_OK) {
    //     return ret;
    // }

    // // CTRL_REG3: Enable interrupt on INT1 pin
    // ret = accel_write_reg(LIS2DH12_CTRL_REG3, 0x40);
    // if (ret != ESP_OK) {
    //     return ret;
    // }

    // // CTRL_REG4: Full-scale = ±2g, Low-resolution mode
    // // 0x00 = 0b00000000
    // // HR = 0 (Low-resolution mode)
    // // FS = 00 (±2g scale)
    // ret = accel_write_reg(LIS2DH12_CTRL_REG4, 0x00);
    // if (ret != ESP_OK) {
    //     return ret;
    // }

    // //Set interrupt pins to active low (CTRL reg 6) so that they are high when not triggered
    // ret = accel_write_reg(LIS2DH12_CTRL_REG6, 0x02);
    // if (ret != ESP_OK) {
    //     return ret;
    // }

    // // Configure the sensor
    // // CTRL_REG1: Enable X, Y, Z axes, 1Hz data rate in low-power mode
    // // 0x8F = 0b00011111
    // // ODR = 0001 low power mode 1Hz
    // // LPEN = 1 (Low-power mode enabled)
    // // Zen = Yen = Xen = 1 (All axes enabled)
    // ret = accel_write_reg(LIS2DH12_CTRL_REG1, 0x1F);
    // if (ret != ESP_OK) {
    //     return ret;
    // }

    ESP_LOGI(TAG, "LIS2DH12 initialized successfully");
    return ESP_OK;
}

esp_err_t accel_read_reg(uint8_t reg_addr, uint8_t *data) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (LIS2DH12_I2C_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg_addr, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (LIS2DH12_I2C_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd, data, I2C_MASTER_NACK);
    i2c_master_stop(cmd);
    
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(I2C_TIMEOUT_MS));
    i2c_cmd_link_delete(cmd);
    
    return ret;
}

esp_err_t accel_write_reg(uint8_t reg_addr, uint8_t data) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (LIS2DH12_I2C_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg_addr, true);
    i2c_master_write_byte(cmd, data, true);
    i2c_master_stop(cmd);
    
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(I2C_TIMEOUT_MS));
    i2c_cmd_link_delete(cmd);
    
    return ret;
}

esp_err_t accel_read_xyz(int16_t *x, int16_t *y, int16_t *z) {
    uint8_t data[6];
    
    // Read all acceleration data in one transaction
    for (int i = 0; i < 6; i++) {
        esp_err_t ret = accel_read_reg(LIS2DH12_OUT_X_L + i, &data[i]);
        if (ret != ESP_OK) {
            return ret;
        }
    }
    
    // Combine high and low bytes
    *x = (int16_t)(data[1] << 8 | data[0]);
    *y = (int16_t)(data[3] << 8 | data[2]);
    *z = (int16_t)(data[5] << 8 | data[4]);
    
    return ESP_OK;
}
