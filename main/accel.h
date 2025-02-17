#ifndef ACCEL_H
#define ACCEL_H

#include <stdint.h>
#include "esp_err.h"

// LIS2DH12TR Register Addresses
#define LIS2DH12_WHO_AM_I          0x0F
#define LIS2DH12_CTRL_REG0         0x1E
#define LIS2DH12_CTRL_REG1         0x20
#define LIS2DH12_CTRL_REG2         0x21
#define LIS2DH12_CTRL_REG3         0x22
#define LIS2DH12_CTRL_REG4         0x23
#define LIS2DH12_CTRL_REG5         0x24
#define LIS2DH12_CTRL_REG6         0x25
#define LIS2DH12_OUT_X_L           0x28
#define LIS2DH12_OUT_X_H           0x29
#define LIS2DH12_OUT_Y_L           0x2A
#define LIS2DH12_OUT_Y_H           0x2B
#define LIS2DH12_OUT_Z_L           0x2C
#define LIS2DH12_OUT_Z_H           0x2D

#define LIS2DH12_I2C_ADDR          0x19

// Function declarations
esp_err_t accel_init(void);
esp_err_t accel_read_reg(uint8_t reg_addr, uint8_t *data);
esp_err_t accel_write_reg(uint8_t reg_addr, uint8_t data);
esp_err_t accel_read_xyz(int16_t *x, int16_t *y, int16_t *z);

#endif // ACCEL_H
