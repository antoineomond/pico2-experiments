#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "sx126x.h"
#include "configuration/apps_configuration.h"
#include "configuration/apps_utilities.h"
#include "common.h"
#include "hardware/clocks.h"
#include "hardware/xosc.h"
#include "hardware/structs/rosc.h"
#include "hardware/rosc.h"
#include <hardware/gpio.h>
#include "hardware/vreg.h"
#include "hardware/pll.h"
#include "hardware/powman.h"
#include "hardware/regs/powman.h"
#include "hardware/ticks.h"
#include "pico/sleep.h"

/*
 Relevant files:
 pico-sdk/src/boards/include/boards/pico2.h
 pico-sdk/src/rp2_common/hardware_spi/spi.c
 pico-sdk/src/rp2_common/pico_stdio/stdio.c
 lorawan-library-for-pico/src/include/pico/lorawan.h
*/

uint32_t iteration_number = 0;
uint8_t  buffer[PAYLOAD_LENGTH];
volatile bool irq_fired = false;
sx126x_chip_status_t* radio_status;
bool received_msg = false;
uint32_t timeout_ms = 200000;
uint16_t irq_mask  = 0b0000001000000010; // Activate RxDone and timeout IRQ (8.5 IRQ Handling)
int16_t rssi_in_dbm;
float SLEEP_RATE = 1000;
float TIME_RATE = 1;

absolute_time_t t;
int64_t ms;
absolute_time_t new_t;
int64_t new_ms;

const spi_connection sx1262_connection = {
	.spio_rx  = PICO_DEFAULT_SPI_RX_PIN,
	.spio_csn = PICO_DEFAULT_SPI_CSN_PIN,
	.spio_sck = PICO_DEFAULT_SPI_SCK_PIN,
	.spio_tx  = PICO_DEFAULT_SPI_TX_PIN
};

void led_blink(int count) {
	for (int i = 0; i < count; i++) {
		gpio_put(PICO_DEFAULT_LED_PIN, 1);
		//sleep_us((int)(1000));
		sleep_us((int)(250000*TIME_RATE));
		gpio_put(PICO_DEFAULT_LED_PIN, 0);
		//sleep_us((int)(1000));
		sleep_us((int)(250000*TIME_RATE));
	}
}

void dio_gpio_callback(uint gpio, uint32_t events)
{
	if (gpio == 20 && events == 8) {
		//new_t = get_absolute_time();
		//new_ms = to_ms_since_boot(new_t);
		//printf("received msg in %lldms\n", new_ms - ms);
		//t = get_absolute_time();
		//ms = to_ms_since_boot(t);
	led_blink(1);
		gpio_acknowledge_irq(gpio, irq_mask);
		sx126x_get_and_clear_irq_status(&sx1262_connection, &irq_mask);
		//print_irq_to_str(irq_mask);
		if(irq_mask & (1 << 1)) { // RxDone
			received_msg = true;
		}
	}
}

static void start_all_ticks(void) {
    uint32_t cycles = clock_get_hz(clk_ref) / MHZ;
		if(cycles <= 0) {
			cycles = 1;
		}
    // Note RP2040 has a single tick generator in the watchdog which serves
    // watchdog, system timer and M0+ SysTick; The tick generator is clocked from clk_ref
    // but is now adapted by the hardware_ticks library for compatibility with RP2350
    // npte: hardware_ticks library now provides an adapter for RP2040

    for (int i = 0; i < (int)TICK_COUNT; ++i) {
        tick_start((tick_gen_num_t)i, cycles);
    }
}

static void restart_all_ticks(void) {
	for (int i = 0; i < (int)TICK_COUNT; ++i) {
			tick_stop((tick_gen_num_t)i);
			while(tick_is_running((tick_gen_num_t)i)) tight_loop_contents();
	}
	start_all_ticks();
}

void leverage_clock_source_lposc() {
	// XOSC needs to be on the clk_ref to accurately count the frequency
	clocks_hw->wake_en1 |= CLOCKS_WAKE_EN1_CLK_SYS_XOSC_BITS;
	xosc_init();
	clock_configure_undivided(clk_ref, CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC, 0, XOSC_HZ);
	restart_all_ticks();
	
	uint lposc_freq = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_LPOSC_CLKSRC)*KHZ;
	clock_configure_undivided(clk_ref, CLOCKS_CLK_REF_CTRL_SRC_VALUE_LPOSC_CLKSRC, 0, lposc_freq);
	clock_configure_undivided(clk_sys, CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLK_REF, 0, lposc_freq);
	TIME_RATE = ((float)lposc_freq)/((float)1*MHZ); // LPOSC isn't fast enough to generate the 1us tick (hardwired value). The TIME_RATE divides any timing to account for this slowness 
	restart_all_ticks();
	pll_deinit(pll_sys);
	pll_deinit(pll_usb);
	rosc_disable();
	xosc_disable();
	clocks_hw->wake_en0 &= ~CLOCKS_WAKE_EN0_CLK_SYS_ROSC_BITS;
	clocks_hw->wake_en1 &= ~CLOCKS_WAKE_EN1_CLK_SYS_XOSC_BITS;
	clocks_hw->wake_en0 &= ~CLOCKS_WAKE_EN0_CLK_SYS_PLL_USB_BITS;
	clocks_hw->wake_en0 &= ~CLOCKS_WAKE_EN0_CLK_SYS_PLL_SYS_BITS;
}

int main() {
	gpio_init(PICO_DEFAULT_LED_PIN);
	gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
	t = get_absolute_time();
	ms = to_ms_since_boot(t);
	//stdio_init_all();
	sleep_ms(500);
	leverage_clock_source_lposc();
	vreg_set_voltage(9);
	
	pico2_spi_init_default(921600);
	//printf("Receiver node\n");
	sx1262_pico2_init(&dio_gpio_callback);
	sx1262_lora_init(&sx1262_connection, irq_mask);
	wait_sx1262_busy();
	sx126x_set_rx(&sx1262_connection, timeout_ms);
	sx126x_rx_buffer_status_t* rx_buffer_status = malloc(sizeof(sx126x_rx_buffer_status_t));
	//sleep_ms(500);
	//leverage_clock_source_lposc();
	uint32_t temperature;
	led_blink(1);
	while(1) {
		if(received_msg) {
			sx126x_get_rx_buffer_status(&sx1262_connection, rx_buffer_status);
			uint8_t read_buffer[rx_buffer_status->pld_len_in_bytes];
			sx126x_read_buffer(&sx1262_connection, rx_buffer_status->buffer_start_pointer, read_buffer, rx_buffer_status->pld_len_in_bytes);
			temperature = read_buffer[0] | (read_buffer[1] << 8) | (read_buffer[2] << 16) | (read_buffer[3] << 24);
			printf("Temp. = %.2fC\n", temperature / 100.0);

			sx126x_set_rx(&sx1262_connection, timeout_ms);
			received_msg = false;
		}
		sleep_us((int)(TIME_RATE*10000));
	}
	return 0;
}
