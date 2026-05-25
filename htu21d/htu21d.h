#ifndef LIBSENSORS_HTU21_D_H
#define LIBSENSORS_HTU21_D_H

#include <stdint.h>
#include <stdbool.h>
#include "osal_i2c.h"

#define HTU21D_I2C_ADDR 0x40

typedef struct
{
    bool is_valid;
    float value;
} htu21d_measurement_t;

typedef struct
{
    const i2c_osal_t *i2c_handler;
} htu21d_t;

int htu21d_init(htu21d_t *self, const i2c_osal_t *i2c_handler);

htu21d_measurement_t htu21d_read_temperature_hold(htu21d_t *self);
htu21d_measurement_t htu21d_read_humidity_hold(htu21d_t *self);
htu21d_measurement_t htu21d_read_temperature_no_hold(htu21d_t *self);
htu21d_measurement_t htu21d_read_humidity_no_hold(htu21d_t *self);

#endif /* LIBSENSORS_HTU21_D_H */
