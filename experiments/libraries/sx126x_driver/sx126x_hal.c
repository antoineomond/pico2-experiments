#include "sx126x_hal.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include "common.h"

extern float TIME_RATE;

static inline void cs_select(const spi_connection* context) {
    //asm volatile("nop \n nop \n nop");
    gpio_put(context->spio_csn, 0);  // Active low
    //asm volatile("nop \n nop \n nop");
}

static inline void cs_deselect(const spi_connection* context) {
    asm volatile("nop \n nop \n nop");
    gpio_put(context->spio_csn, 1);
    asm volatile("nop \n nop \n nop");
}

static void write_register(const spi_connection* context, const uint8_t* reg, const uint16_t reg_length, const uint8_t* data, uint16_t len) {
    gpio_put(context->spio_csn, 0);  // Active low
    spi_write_blocking(spi_default, reg, reg_length);
    sleep_us((int)(TIME_RATE*10000));
    spi_write_blocking(spi_default, data, len);
    gpio_put(context->spio_csn, 1);  // Active low
    sleep_us((int)(TIME_RATE*10000));
}

static void read_registers(const spi_connection* context, const uint8_t* reg, const uint16_t reg_length, uint8_t *buf, uint16_t len) {
    // For this particular device, we send the device the register we want to read
    // first, then subsequently read from the device. The register is auto incrementing
    // so we don't need to keep sending the register we want, just the first.
    gpio_put(context->spio_csn, 0);  // Active low
    spi_write_blocking(spi_default, reg, reg_length);
    sleep_us((int)(TIME_RATE*10000));
    spi_read_blocking(spi_default, 0, buf, len);
    gpio_put(context->spio_csn, 1);  // Active low
    sleep_us((int)(TIME_RATE*10000));
}

sx126x_hal_status_t sx126x_hal_write( const void* context, const uint8_t* command, const uint16_t command_length,
                                      const uint8_t* data, const uint16_t data_length )
{
	spi_connection* c = (spi_connection*) context;
	write_register(c, command, command_length, data, data_length);
  return SX126X_HAL_STATUS_OK;
}

sx126x_hal_status_t sx126x_hal_read( const void* context, const uint8_t* command, const uint16_t command_length,
                                     uint8_t* data, const uint16_t data_length )
{
	read_registers(context, command, command_length, data, data_length);
  return SX126X_HAL_STATUS_OK;
}

sx126x_hal_status_t sx126x_hal_wakeup( const void* context )
{
	spi_connection* c = (spi_connection*) context;
	cs_deselect(c);
	sleep_us((int)(TIME_RATE*1000000));
	cs_select(c);
  return SX126X_HAL_STATUS_OK;
}

sx126x_hal_status_t sx126x_hal_reset( const void* context )
{
	//spi_connection* c = (spi_connection*) context;
  //gpio_put(c->gp_20, 0);
	//sleep_us((int)(TIME_RATE*1000000));
  //gpio_put(c->gp_20, 1);
	//printf("Unimplemented"); // TODO: to implement
  return SX126X_HAL_STATUS_OK;
}
