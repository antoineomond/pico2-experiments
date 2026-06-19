#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "bme680_pico2.h"

// BME680
uint8_t BME_SPI_RX_PIN = 12;
uint8_t BME_SPI_SCK_PIN = 10;
uint8_t BME_SPI_TX_PIN = 11;
uint8_t BME_SPI_CSN_PIN = 13;
#define READ_BIT 0x80
uint16_t par_t1, par_t2;
uint8_t par_t3;
uint16_t par_h1, par_h2;
uint8_t par_h3, par_h4, par_h5, par_h6, par_h7;
uint16_t par_p1, par_p2, par_p4, par_p5, par_p8, par_p9;
uint8_t par_p3, par_p6, par_p7, par_p10;
uint16_t par_g2;
uint8_t par_g1, par_g3, res_heat_range;
int8_t res_heat_val;
uint8_t range_switching_error, gas_range;
uint32_t const_array1_int[16] = {2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2126008810, 2147483647, 2130303777, 2147483647, 2147483647, 2143188679, 2136746228, 2147483647, 2126008810, 2147483647, 2147483647};
uint32_t const_array2_int[16] = {4096000000, 2048000000, 1024000000, 512000000, 255744255, 127110228, 64000000, 32258064, 16016016, 8000000, 4000000, 2000000, 1000000, 500000, 250000, 125000};
int32_t t_fine;
int32_t temp_comp;
int32_t amb_temp;
uint16_t target_temp = 100; // too low (below 200)
extern float TIME_RATE;
//int32_t calc_temp(int16_t par_t1, int16_t par_t2, int8_t par_t3, int32_t temp_adc) {
//		// From pico-examples/spi/bme280_spi/bme280_spi.c
//    int32_t var1, var2, T;
//    var1 = ((((temp_adc >> 3) - ((int32_t) par_t1 << 1))) * ((int32_t) par_t2)) >> 11;
//    var2 = (((((temp_adc >> 4) - ((int32_t) par_t1)) * ((temp_adc >> 4) - ((int32_t) par_t1))) >> 12) * ((int32_t) par_t3))
//            >> 14;
//
//    t_fine = var1 + var2;
//    T = (t_fine * 5 + 128) >> 8;
//    return T;
//}

int32_t calc_temp(int16_t par_t1, int16_t par_t2, int8_t par_t3, int32_t temp_adc) {
		// From bme68x_driver/bme68x.c
    int64_t var1;
    int64_t var2;
    int64_t var3;
    int16_t calc_temp;

    /*lint -save -e701 -e702 -e704 */
    var1 = ((int32_t)temp_adc >> 3) - ((int32_t)par_t1 << 1);
    var2 = (var1 * (int32_t)par_t2) >> 11;
    var3 = ((var1 >> 1) * (var1 >> 1)) >> 12;
    var3 = ((var3) * ((int32_t)par_t3 << 4)) >> 14;
    t_fine = (int32_t)(var2 + var3);
    calc_temp = (int32_t)(((t_fine * 5) + 128) >> 8);
		temp_comp = calc_temp;
		amb_temp = temp_comp / 100;

    /*lint -restore */
    return calc_temp;
}

uint8_t calc_hum(uint16_t par_h1, uint16_t par_h2, uint8_t par_h3, uint8_t par_h4, uint8_t par_h5, uint8_t par_h6, uint8_t par_h7, uint16_t hum_adc) {
	// From datasheet
	int32_t temp_scaled = (int32_t)temp_comp;
	int32_t var1 = (int32_t)hum_adc - (int32_t)((int32_t)par_h1 << 4) - (((temp_scaled * (int32_t)par_h3) / ((int32_t)100)) >> 1);
	int32_t var2 = ((int32_t)par_h2 * (((temp_scaled * (int32_t)par_h4) / ((int32_t)100)) + (((temp_scaled * ((temp_scaled * (int32_t)par_h5) / ((int32_t)100))) >> 6) / ((int32_t)100)) + ((int32_t)(1 << 14)))) >> 10;
	int32_t var3 = var1 * var2;
	int32_t var4 = (((int32_t)par_h6 << 7) + ((temp_scaled * (int32_t)par_h7) / ((int32_t)100))) >> 4;
	int32_t var5 = ((var3 >> 14) * (var3 >> 14)) >> 10;
	int32_t var6 = (var4 * var5) >> 1;
	int32_t hum_comp = (((var3 + var6) >> 10) * ((int32_t) 1000)) >> 12;
	return hum_comp;
}

uint32_t calc_press(int32_t pres_adc) {
    int32_t var1;
    int32_t var2;
    int32_t var3;
    int32_t pressure_comp;

    /* This value is used to check precedence to multiplication or division
     * in the pressure compensation equation to achieve least loss of precision and
     * avoiding overflows.
     * i.e Comparing value, pres_ovf_check = (1 << 31) >> 1
     */
    const int32_t pres_ovf_check = INT32_C(0x40000000);

    /*lint -save -e701 -e702 -e713 */
    var1 = (((int32_t)t_fine) >> 1) - 64000;
    var2 = ((((var1 >> 2) * (var1 >> 2)) >> 11) * (int32_t)par_p6) >> 2;
    var2 = var2 + ((var1 * (int32_t)par_p5) << 1);
    var2 = (var2 >> 2) + ((int32_t)par_p4 << 16);
    var1 = (((((var1 >> 2) * (var1 >> 2)) >> 13) * ((int32_t)par_p3 << 5)) >> 3) +
           (((int32_t)par_p2 * var1) >> 1);
    var1 = var1 >> 18;
    var1 = ((32768 + var1) * (int32_t)par_p1) >> 15;
    pressure_comp = 1048576 - pres_adc;
    pressure_comp = (int32_t)((pressure_comp - (var2 >> 12)) * ((uint32_t)3125));
    if (pressure_comp >= pres_ovf_check)
    {
        pressure_comp = ((pressure_comp / var1) << 1);
    }
    else
    {
        pressure_comp = ((pressure_comp << 1) / var1);
    }

    var1 = ((int32_t)par_p9 * (int32_t)(((pressure_comp >> 3) * (pressure_comp >> 3)) >> 13)) >> 12;
    var2 = ((int32_t)(pressure_comp >> 2) * (int32_t)par_p8) >> 13;
    var3 =
        ((int32_t)(pressure_comp >> 8) * (int32_t)(pressure_comp >> 8) * (int32_t)(pressure_comp >> 8) *
         (int32_t)par_p10) >> 17;
    pressure_comp = (int32_t)(pressure_comp) + ((var1 + var2 + var3 + ((int32_t)par_p7 << 7)) >> 4);

    /*lint -restore */
    return (uint32_t)pressure_comp;
}

uint8_t calc_res_heat_x() {
	int32_t var1 = (((int32_t)amb_temp * par_g3) / 10) << 8;
	int32_t var2 = (par_g1 + 784) * (((((par_g2 + 154009) * target_temp * 5) / 100) + 3276800) / 10);
	int32_t var3 = var1 + (var2 >> 1);
	int32_t var4 = (var3 / (res_heat_range + 4));
	int32_t var5 = (131 * res_heat_val) + 65536;
	int32_t res_heat_x100 = (int32_t)(((var4 / var5) - 250) * 34);
	uint8_t res_heat_x = (uint8_t)((res_heat_x100 + 50) / 100);
	return res_heat_x;
}

int32_t calc_gas(uint16_t gas_adc) {
	int64_t var1 = (int64_t)(((1340 + (5 * (int64_t)range_switching_error)) * ((int64_t)const_array1_int[gas_range])) >> 16);
	int64_t var2 = (int64_t)(gas_adc << 15) - (int64_t)(1 << 24) + var1;
	int32_t gas_res = (int32_t)((((int64_t)(const_array2_int[gas_range] * (int64_t)var1) >> 9) + (var2 >> 1)) / var2);
	return gas_res;
}

#define CS_PIN 5

static inline void cs_select() {
    asm volatile("nop \n nop \n nop");
    gpio_put(CS_PIN, 0);  // Active low
    asm volatile("nop \n nop \n nop");
}

static inline void cs_deselect() {
    asm volatile("nop \n nop \n nop");
    gpio_put(CS_PIN, 1);
    asm volatile("nop \n nop \n nop");
}

static void write_register(uint8_t reg, uint8_t data) {
    uint8_t buf[2];
    buf[0] = reg & 0x7f;  // remove read bit as this is a write
    buf[1] = data;
    cs_select();
    spi_write_blocking(spi_default, buf, 2);
    cs_deselect();
    sleep_us((int)(TIME_RATE*10000));
}

static void read_registers(uint8_t reg, uint8_t *buf, uint16_t len) {
    // For this particular device, we send the device the register we want to read
    // first, then subsequently read from the device. The register is auto incrementing
    // so we don't need to keep sending the register we want, just the first.
    reg |= READ_BIT;
    cs_select();
    spi_write_blocking(spi_default, &reg, 1);
    sleep_us((int)(TIME_RATE*10000));
    spi_read_blocking(spi_default, 0, buf, len);
    cs_deselect();
    sleep_us((int)(TIME_RATE*10000));
}

void bme680_pico2_init() {
	// Initialisation BME680
	gpio_init(CS_PIN);
	gpio_set_dir(CS_PIN, GPIO_OUT);
	
	write_register(0x73, 0); // Switch to page 0
	sleep_us((int)(TIME_RATE*100000));
	uint8_t id;
	read_registers(0x50, &id, 1);
	//printf("Chip ID is 0x%x\n", id); // Should be 0x61
	//write_register(0x73, 0b00010000); // Switch to page 1
	
	// Read temperature calibration parameters
	//printf("Reading temperature calibration parameters\n");
	sleep_us((int)(TIME_RATE*50000));
	uint8_t par_t1_data[2];
	read_registers(0xE9, par_t1_data, 2);
	par_t1 = par_t1_data[0] | (par_t1_data[1] << 8);
	uint8_t par_t2_data[2];
	read_registers(0x8A, par_t2_data, 2);
	par_t2 = par_t2_data[0] | (par_t2_data[1] << 8);
	read_registers(0x8C, &par_t3, 1);

	// Read humidity calibration parameters
	//printf("Reading humidity calibration parameters\n");
	sleep_us((int)(TIME_RATE*50000)); 
	uint8_t par_h1_lsb[1];
	read_registers(0xE2, par_h1_lsb, 1);
	uint8_t par_h1_msb[1];
	read_registers(0xE3, par_h1_msb, 1);
	par_h1 = (((par_h1_lsb[0] & 0b00001111) << 4) | (par_h1_msb[0] << 8)) >> 4; // TODO: check
	uint8_t par_h2_lsb[1];
	read_registers(0xE2, par_h2_lsb, 1);
	uint8_t par_h2_msb[1];
	read_registers(0xE1, par_h2_msb, 1);
	par_h2 = ((par_h2_lsb[0] & 0b11110000) | (par_h2_msb[0] << 8)) >> 4; // TODO: check
	read_registers(0xE4, &par_h3, 1);
	read_registers(0xE5, &par_h4, 1);
	read_registers(0xE6, &par_h5, 1);
	read_registers(0xE7, &par_h6, 1);
	read_registers(0xE8, &par_h7, 1);
	//uint8_t hum_adc_lsb[1];
	//read_registers(0x26, hum_adc_lsb, 1);
	//uint8_t hum_adc_msb[1];
	//read_registers(0x25, hum_adc_msb, 1);
	//uint16_t hum_adc = hum_adc_lsb[0] | (hum_adc_msb[0] << 8);
	
	// Read pressure calibration parameters
	//printf("Reading pressure calibration parameters\n");
	sleep_us((int)(TIME_RATE*50000));
	uint8_t par_p1_lsb[1];
	read_registers(0x8E, par_p1_lsb, 1);
	uint8_t par_p1_msb[1];
	read_registers(0x8F, par_p1_msb, 1);
	par_p1 = par_p1_lsb[0] | (par_p1_msb[0] << 8);
	uint8_t par_p2_lsb[1];
	read_registers(0x90, par_p2_lsb, 1);
	uint8_t par_p2_msb[1];
	read_registers(0x91, par_p2_msb, 1);
	par_p2 = par_p2_lsb[0] | (par_p2_msb[0] << 8);
	read_registers(0x92, &par_p3, 1);
	uint8_t par_p4_lsb[1];
	read_registers(0x94, par_p4_lsb, 1);
	uint8_t par_p4_msb[1];
	read_registers(0x95, par_p4_msb, 1);
	par_p4 = par_p4_lsb[0] | (par_p4_msb[0] << 8);
	uint8_t par_p5_lsb[1];
	read_registers(0x96, par_p5_lsb, 1);
	uint8_t par_p5_msb[1];
	read_registers(0x97, par_p5_msb, 1);
	par_p5 = par_p5_lsb[0] | (par_p5_msb[0] << 8);
	read_registers(0x99, &par_p6, 1);
	read_registers(0x98, &par_p7, 1);
	uint8_t par_p8_lsb[1];
	read_registers(0x9C, par_p8_lsb, 1);
	uint8_t par_p8_msb[1];
	read_registers(0x9D, par_p8_msb, 1);
	par_p8 = par_p8_lsb[0] | (par_p8_msb[0] << 8);
	uint8_t par_p9_lsb[1];
	read_registers(0x9E, par_p9_lsb, 1);
	uint8_t par_p9_msb[1];
	read_registers(0x9F, par_p9_msb, 1);
	par_p9 = par_p9_lsb[0] | (par_p9_msb[0] << 8);
	read_registers(0xA0, &par_p10, 1);

	// Reading gas calibration parameters
	read_registers(0xED, &par_g1, 1);
	uint8_t par_g2_data[2];
	read_registers(0xEB, par_g2_data, 2);
	par_g2 = par_g2_data[0] | (par_g2_data[1] << 8);
	read_registers(0xEE, &par_g3, 1);
	
	write_register(0x73, 0b00010000); // Switch to page 1
	
	read_registers(0x02, &res_heat_range, 1);
	res_heat_range = res_heat_range >> 4;
	uint8_t res_heat_val_uint;
	read_registers(0x00, &res_heat_val_uint, 1);
	res_heat_val = (int8_t)res_heat_val_uint;
	
	// Set oversampling to 1x for humidity
	//printf("Setting oversampling parameters\n");
	sleep_us((int)(TIME_RATE*50000));
	uint8_t ctrl_hum;
	read_registers(0x72, &ctrl_hum, 1);
	//write_register(0x72, (ctrl_hum & 0b11111000) | 0b00000001); // Set oversampling to 1x
	write_register(0x72, (ctrl_hum & 0b11111000)); // Set oversampling to 0x (deactivation of hum)
	// Set oversampling to 2x for temperature and 16x for pressure (present on same byte)
	uint8_t ctrl_meas;
	read_registers(0x74, &ctrl_meas, 1);
	//write_register(0x74, (ctrl_meas & 0b00000011) | 0b01010100); // Set oversampling to 2x and 16x
	write_register(0x74, (ctrl_meas & 0b00000011) | 0b01000000); // Set oversampling to 0x for press (deactivation of press)

	// Gas parameters
	// Activation/deactivation
	//printf("Select previously defined heater settings and set run_gas to 1\n");
	sleep_us((int)(TIME_RATE*50000));
	uint8_t ctrl_gas_1;
	read_registers(0x71, &ctrl_gas_1, 1);
	//write_register(0x71, (ctrl_gas_1 & 0b11100000) | 0b00010000); // Activate gas and select heater setting 0 
	write_register(0x71, ctrl_gas_1 & 0b11100000); // Deactivate gas
	
	// heater params
	//write_register(0x64, 0x59); // Set gas_wait_0<7:0> to 0x59 (100 ms duration)
	//uint8_t res_heat_x = calc_res_heat_x();
	//write_register(0x5A, res_heat_x); // Set res_heat_0<7:0> to computed value
}

void trigger_bme680_msrmt(uint32_t* measurements) {

	write_register(0x73, 0b00010000); // Switch to page 1
	// Trigger single measurement
	uint8_t ctrl_meas;
	read_registers(0x74, &ctrl_meas, 1);
	write_register(0x74, ctrl_meas | 0b00000001);

	// Wait until data is ready
	uint8_t new_data_0;
	uint8_t all_good = 0;
	//sleep_us((int)(TIME_RATE*500000));
	while (all_good != 1) {
		all_good = 1;
		// Check msrmt status
		read_registers(0x1D, &new_data_0, 1);
		if (((new_data_0 & 0b10000000) >> 7) != 1) {
			//printf("Data not ready yet\n");
			all_good = 0;
		}
		// check gas
		//uint8_t ctrl_gas;
		//read_registers(0x2B, &ctrl_gas, 1);
		//if(((ctrl_gas & 0b00100000) >> 5) != 1) {
		//	printf("Gas measurement invalid\n");
		//	all_good = 0;
		//}
		//if(((ctrl_gas & 0b00010000) >> 4) != 1) {
		//	printf("Heating time not sufficient or too high temperature\n");
		//	all_good = 0;
		//}
		sleep_us((int)(TIME_RATE*1000));
	}
	// Read temperature data
	//printf("Read temperature data\n");
	//sleep_us((int)(TIME_RATE*50000));
	uint8_t temp_data[3];
	read_registers(0x22, temp_data, 3);
	uint32_t temp_adc = (temp_data[0] << 12) | (temp_data[1] << 4) | (temp_data[2] >> 4);  // TODO: check
	uint32_t temperature = calc_temp(par_t1, par_t2, par_t3, temp_adc);
	measurements[0] = temperature;

	// Read humidity data
	//uint8_t hum_data[2];
	//read_registers(0x25, hum_data, 2);
	//uint16_t hum_adc = hum_data[0] | (hum_data[1] << 8);
	//uint32_t humidity = calc_hum(par_h1, par_h2, par_h3, par_h4, par_h5, par_h6, par_h7, hum_adc);
	//measurements[1] = humidity;

	//// Read pressure data
	//uint8_t press_adc_xlsb[1];
	//read_registers(0x21, press_adc_xlsb, 1);
	//uint8_t press_adc_lsb[1];
	//read_registers(0x20, press_adc_lsb, 1);
	//uint8_t press_adc_msb[1];
	//read_registers(0x1F, press_adc_msb, 1);
	//int32_t press_adc = (press_adc_xlsb[0] | (press_adc_lsb[0] << 8) | (press_adc_msb[0] << 16)) >> 4;
	//uint32_t pressure = calc_press(press_adc);
	//measurements[2] = pressure;

	//// Read gas data
	//read_registers(0x04, &range_switching_error, 1);
	//read_registers(0x2B, &gas_range, 1);
	//gas_range = gas_range & 0b00001111;
	//uint8_t gas_adc_data[2];
	//read_registers(0x2B, gas_adc_data, 2);
	//uint16_t gas_adc = (gas_adc_data[0] | (gas_adc_data[1] << 8)) >> 6;
	//int32_t gas_resistance = calc_gas(gas_adc);
	//measurements[3] = gas_resistance;
	
	//sleep_us((int)(TIME_RATE*1000000));
}
