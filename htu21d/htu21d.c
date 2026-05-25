#include "htu21d.h"
#include "crc.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define HTU21D_I2C_ADDR 0x40

#define TRIGGER_TEMP_HOLD 0xE3
#define TRIGGER_HUMID_HOLD 0xE5

// Non-hold master commands
#define TRIGGER_TEMP_NO_HOLD 0xF3
#define TRIGGER_HUMID_NO_HOLD 0xF5

#define MEASUREMENT_MASK 0xFC
#define MEASUREMENT_TYPE_MASK 0x02
#define TEMPERATURE_MEASUREMENT 0x00
#define HUMIDITY_MEASUREMENT 0x02

#define HTU21D_MEAS_DELAY_US 100000 // 100ms max measurement time

int htu21d_init(htu21d_t *self, const i2c_osal_t *i2c_handler)
{
    if (!i2c_handler)
    {
        return -1;
    }

    self->i2c_handler = i2c_handler;

    return 0;
}

/****************** Hold master commands ******************/
static htu21d_measurement_t get_measurement_hold(htu21d_t *self, uint8_t command)
{
    htu21d_measurement_t res;
    uint8_t data[3];

    if (self == NULL || self->i2c_handler == NULL)
    {
        goto err_out;
    }

    if (self->i2c_handler->write_read(self->i2c_handler->ctx, HTU21D_I2C_ADDR, &command, 1, data, 3) < 0)
    {
        goto err_out;
    }

    uint8_t computed_crc = compute_crc8(data, 2);

    if (computed_crc != data[2])
    {
        goto err_out;
    }

    uint16_t raw = (data[0] << 8) | (data[1] & MEASUREMENT_MASK);

    if ((data[1] & MEASUREMENT_TYPE_MASK) == TEMPERATURE_MEASUREMENT)
    {
        // Temperature measurement
        res.is_valid = true;
        res.value = -46.85 + (175.72 * raw) / 65536.0;
    }
    else
    {
        // Humidity measurement
        res.is_valid = true;
        res.value = -6.0 + (125.0 * raw) / 65536.0;
    }
    return res;

err_out:
    res.is_valid = false;
    return res;
}

htu21d_measurement_t htu21d_read_temperature_hold(htu21d_t *self)
{
    return get_measurement_hold(self, TRIGGER_TEMP_HOLD);
}

htu21d_measurement_t htu21d_read_humidity_hold(htu21d_t *self)
{
    return get_measurement_hold(self, TRIGGER_HUMID_HOLD);
}

/****************** Non-hold master commands ******************/
static htu21d_measurement_t get_measurement_no_hold(htu21d_t *self, uint8_t command, uint8_t meas_type)
{
    htu21d_measurement_t res = {.is_valid = false, .value = 0};
    uint8_t data[3];

    if (!self || !self->i2c_handler)
        return res;

    /* trigger measurement */
    if (self->i2c_handler->write(self->i2c_handler->ctx, HTU21D_I2C_ADDR, &command, 1) < 0)
    {
        return res;
    }

    /* wait for conversion to finish */
    self->i2c_handler->delay_ms(self->i2c_handler->ctx, HTU21D_MEAS_DELAY_US / 1000);

    /* read measurement */
    if (self->i2c_handler->read(self->i2c_handler->ctx, HTU21D_I2C_ADDR, data, 3) < 0)
    {
        return res;
    }

    /* verify CRC */
    uint8_t computed_crc = compute_crc8(data, 2);
    if (computed_crc != data[2])
    {
        return res;
    }

    /* convert raw value */
    uint16_t raw = (data[0] << 8) | (data[1] & MEASUREMENT_MASK);

    if (meas_type == TEMPERATURE_MEASUREMENT)
        res.value = -46.85 + (175.72 * raw) / 65536.0;
    else
        res.value = -6.0 + (125.0 * raw) / 65536.0;

    res.is_valid = true;
    return res;
}

htu21d_measurement_t htu21d_read_temperature_no_hold(htu21d_t *self)
{
    return get_measurement_no_hold(self, TRIGGER_TEMP_NO_HOLD, TEMPERATURE_MEASUREMENT);
}

htu21d_measurement_t htu21d_read_humidity_no_hold(htu21d_t *self)
{
    return get_measurement_no_hold(self, TRIGGER_HUMID_NO_HOLD, HUMIDITY_MEASUREMENT);
}
