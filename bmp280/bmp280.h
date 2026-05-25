#ifndef LIBSENSORS_BMP280_H
#define LIBSENSORS_BMP280_H

#include "osal_i2c.h"
#include <stdint.h>

#define BMP280_I2C_ADDR 0x76

// Calibration parameters structure
typedef struct
{
    uint16_t dig_T1;
    int16_t dig_T2, dig_T3;
    uint16_t dig_P1;
    int16_t dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
} bmp280_calib_data;

typedef struct
{
    bmp280_calib_data calib;
    const i2c_osal_t *i2c_handler;
} bmp280_t;

int bmp280_init(bmp280_t *self, const i2c_osal_t *i2c_handler);

// Function to calculate temperature and pressure using the calibration parameters
int bmp280_get_measurement(bmp280_t *self,
                           float *temperature, float *pressure);

#endif /* LIBSENSORS_BMP280_H */
