#ifndef LIBSENSORS_I2C_H
#define LIBSENSORS_I2C_H

#include <stddef.h>
#include <stdint.h>

typedef int (*osal_i2c_write_fn)(void *ctx, uint8_t addr, const uint8_t *data, size_t len);

typedef int (*osal_i2c_read_fn)(void *ctx, uint8_t addr, uint8_t *data, size_t len);

typedef int (*osal_i2c_write_read_fn)(void *ctx, uint8_t addr, uint8_t *tx_data, size_t tx_len,
                                      uint8_t *rx_data, size_t rx_len);

typedef void (*osal_i2c_delay_ms_fn)(void *ctx, uint32_t ms);

typedef struct {
    void *ctx;

    osal_i2c_write_fn write;
    osal_i2c_read_fn read;
    osal_i2c_write_read_fn write_read;
    osal_i2c_delay_ms_fn delay_ms;

} i2c_osal_t;

#endif /* LIBSENSORS_I2C_H */
