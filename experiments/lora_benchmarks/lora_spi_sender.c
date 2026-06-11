#include <pico.h>
#include <hardware/gpio.h>
#include <pico/time.h>
#include "hardware/clocks.h"
#include "hardware/xosc.h"
#include "hardware/structs/rosc.h"
#include "hardware/rosc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"
#include "sx126x.h"
#include "sx126x_hal.h"
#include "printers/sx126x_str.h"
#include "configuration/apps_configuration.h"
#include "configuration/apps_common.h"
#include "configuration/apps_utilities.h"
#include "common.h"
#include "bme680_pico2.c"
//#include "pico/aon_timer.h"
//#include <time.h>
//#include <sys/time.h>
//#include <time.h>
#include "hardware/pll.h"
#include "pico/sleep.h"
#include "hardware/powman.h"
#include "hardware/ticks.h"
#include "hardware/vreg.h"
#ifdef __riscv
#include "hardware/riscv.h"
#else
// For scb_hw so we can enable deep sleep
#include "hardware/structs/scb.h"
#endif

// For wfi
#include "hardware/sync.h"
#include "pico/runtime_init.h"

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
float TIME_RATE = 1; // LPOSC isn't fast enough to generate the 1us tick (hardwired). The TIME_RATE divide any timing to account for this slowness 

const spi_connection sx1262_connection = {
	.spio_rx  = PICO_DEFAULT_SPI_RX_PIN,
	.spio_csn = PICO_DEFAULT_SPI_CSN_PIN,
	.spio_sck = PICO_DEFAULT_SPI_SCK_PIN,
	.spio_tx  = PICO_DEFAULT_SPI_TX_PIN
};

const sx126x_pa_cfg_params_t pa_cfg = {
	.pa_duty_cycle = 0x02,  // 0x02 = +14 dBm, 0x04 = +22 dBm
	.hp_max				 = 0x02,  // Also related to dBm 
	.device_sel		 = 0,     // SX1262 selection (1 is SX1261)
	.pa_lut				 = 0x01   // Reserved, always 0x01
};

const sx1262_tx_params tx_params = {
	.power = 0x16,              // Default +14 dBm if low power PA is selected
	.ramp_time = PA_RAMP_TIME
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

bool tx_done = false;
void dio_gpio_callback(uint gpio, uint32_t events)
{
	//printf("dio_gpio_callback\n");
	//sleep_us((int)(TIME_RATE*50000));
	if (gpio == 20 && events == 8) {
		sx126x_irq_mask_t irq_mask;
		sx126x_clear_irq_status(&sx1262_connection, 0b1111111111111111);
		sx126x_get_irq_status(&sx1262_connection, &irq_mask);
		tx_done = true;
		//led_blink(1);
		
		//printf("IRQ received: ");
		//print_irq_to_str(irq_mask);
		//printf("\n");
	}
}

static void sleep_callback(void) {
		//led_blink(3);
}

void assign_clk_src() {
	clock_configure_undivided(clk_ref, CLOCKS_CLK_REF_CTRL_SRC_VALUE_LPOSC_CLKSRC, 0, _u(32768));
	clock_configure_undivided(clk_sys,
									CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLK_REF,
									0,
									SYS_CLK_HZ);
	clock_configure_undivided(clk_usb,
									0, // No GLMUX
									CLOCKS_CLK_USB_CTRL_AUXSRC_VALUE_ROSC_CLKSRC_PH,
									USB_CLK_HZ);
	clock_configure_undivided(clk_adc,
									0, // No GLMUX
									CLOCKS_CLK_USB_CTRL_AUXSRC_VALUE_ROSC_CLKSRC_PH,
									USB_CLK_HZ);
	clock_configure_undivided(clk_peri,
									0,
									CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS,
									SYS_CLK_HZ);
	clock_configure_undivided(clk_hstx,
									0,
									CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS,
									SYS_CLK_HZ);
}

void deactivate_pll() {
	pll_deinit(pll_usb);
	pll_deinit(pll_sys);
}

void deactivate_xosc() {
	xosc_disable();
}

//static void start_all_ticks(void) {
//    uint32_t cycles = 1;
//    // Note RP2040 has a single tick generator in the watchdog which serves
//    // watchdog, system timer and M0+ SysTick; The tick generator is clocked from clk_ref
//    // but is now adapted by the hardware_ticks library for compatibility with RP2350
//    // npte: hardware_ticks library now provides an adapter for RP2040
//
//    for (int i = 0; i < (int)TICK_COUNT; ++i) {
//        tick_start((tick_gen_num_t)i, cycles);
//    }
//}

// For LPOSC, override runtime_init_clocks defined in pico-sdk/src/rp2_common/pico_runtime_init/runtime_init_clocks.c
//void runtime_init_clocks(void) {
//	// Note: These need setting *before* the ticks are started
//	// Disable resus that may be enabled from previous software
//	clocks_hw->resus.ctrl = 0;
//
//	// clk_ref aux select is 0 because:
//	//
//	// - RP2350: there is an aux mux, but we are selecting one of the
//	//   non-aux inputs to the glitchless mux, so the aux select doesn't
//	//   matter. The value of 0 here happens to be the sys PLL.
//
//	hw_clear_bits(&clocks_hw->clk[clk_sys].ctrl, CLOCKS_CLK_SYS_CTRL_SRC_BITS);
//	while (clocks_hw->clk[clk_sys].selected != 0x1)
//			tight_loop_contents();
//	hw_clear_bits(&clocks_hw->clk[clk_ref].ctrl, CLOCKS_CLK_REF_CTRL_SRC_BITS);
//	while (clocks_hw->clk[clk_ref].selected != 0x1)
//			tight_loop_contents();
//
//clock_configure_undivided(clk_ref,
//									CLOCKS_CLK_REF_CTRL_SRC_VALUE_LPOSC_CLKSRC,
//									0,
//								  _u(32768));
//
//#if SYS_CLK_VREG_VOLTAGE_AUTO_ADJUST && defined(SYS_CLK_VREG_VOLTAGE_MIN)
//	if (vreg_get_voltage() < SYS_CLK_VREG_VOLTAGE_MIN) {
//			vreg_set_voltage(SYS_CLK_VREG_VOLTAGE_MIN);
//			// wait for voltage to settle; must use CPU cycles as TIMER is not yet clocked correctly
//			busy_wait_at_least_cycles((uint32_t)((SYS_CLK_VREG_VOLTAGE_AUTO_ADJUST_DELAY_US * (uint64_t)XOSC_HZ) / 1000000));
//	}
//#endif
//
//	clock_configure_undivided(clk_sys,
//									CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLK_REF,
//									0,
//								  _u(32768));
//	clock_configure_undivided(clk_usb,
//									0, // No GLMUX
//									CLOCKS_CLK_USB_CTRL_AUXSRC_VALUE_ROSC_CLKSRC_PH,
//									_u(32768));
//	clock_configure_undivided(clk_adc,
//									0, // No GLMUX
//									CLOCKS_CLK_ADC_CTRL_AUXSRC_VALUE_ROSC_CLKSRC_PH,
//									_u(32768));
//	clock_configure_undivided(clk_peri,
//									0,
//									CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS,
//									_u(32768));
//
//#if HAS_HSTX
//	// CLK_HSTX = clk_sys. Transmit bit clock for the HSTX peripheral.
//	clock_configure_undivided(clk_hstx,
//									0,
//									CLOCKS_CLK_HSTX_CTRL_AUXSRC_VALUE_CLK_SYS,
//									_u(32768));
//#endif
//
//	// Finally, all clocks are configured so start the ticks
//	// The ticks use clk_ref so now that is configured we can start them
//	start_all_ticks();
//}

static void processor_deep_sleep(void) {
    // Enable deep sleep at the proc
#ifdef __riscv
    uint32_t bits = RVCSR_MSLEEP_POWERDOWN_BITS;
    if (!get_core_num()) {
        bits |= RVCSR_MSLEEP_DEEPSLEEP_BITS;
    }
    riscv_set_csr(RVCSR_MSLEEP_OFFSET, bits);
#else
    scb_hw->scr |= ARM_CPU_PREFIXED(SCR_SLEEPDEEP_BITS);
#endif
}

void go_dormant_until(struct timespec *ts, aon_timer_alarm_handler_t callback) {
	uint64_t restore_ms = powman_timer_get_ms();
	powman_timer_set_1khz_tick_source_lposc();
	powman_timer_set_ms(restore_ms);

	clocks_hw->sleep_en0 = CLOCKS_SLEEP_EN0_CLK_REF_POWMAN_BITS;
	clocks_hw->sleep_en1 = 0x0;
	
	// Set the AON timer to wake up the proc from dormant mode
	aon_timer_enable_alarm(ts, callback, true);

	// Enable deep sleep at the proc
	processor_deep_sleep();

	// Go dormant
	//rosc_set_dormant();
	__wfi();
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

void leverage_deactivate_brownout_detection() {
	vreg_disable_voltage_limit();
	powman_clear_bits(&powman_hw->bod, 0x000001f1);
}

void leverage_reduced_wake_en_gating() {
	//clocks_hw->wake_en0 = 0xbdf00001;
	//clocks_hw->wake_en1 = 0xffffffff;
	clocks_hw->wake_en0 = CLOCKS_WAKE_EN0_CLK_SYS_CLOCKS_BITS |
												CLOCKS_WAKE_EN0_CLK_SYS_PLL_USB_BITS |
												CLOCKS_WAKE_EN0_CLK_SYS_PLL_SYS_BITS | // Note: deactivating this doesn't deactivate the PLL_SYS from generating a signal, but it prevents its reconfiguration
												CLOCKS_WAKE_EN0_CLK_SYS_PIO2_BITS | // Added for multicore
												CLOCKS_WAKE_EN0_CLK_REF_POWMAN_BITS |
												CLOCKS_WAKE_EN0_CLK_SYS_POWMAN_BITS |
												CLOCKS_WAKE_EN0_CLK_SYS_RESETS_BITS |	
												CLOCKS_WAKE_EN0_CLK_SYS_ROM_BITS |  // Added for multicore
												CLOCKS_WAKE_EN0_CLK_SYS_ROSC_BITS |
												CLOCKS_WAKE_EN0_CLK_SYS_IO_BITS |
												CLOCKS_WAKE_EN0_CLK_SYS_PSM_BITS |  // Added for multicore
												CLOCKS_WAKE_EN0_CLK_SYS_SIO_BITS;
	clocks_hw->wake_en1 = CLOCKS_WAKE_EN1_CLK_SYS_SPI1_BITS |
												CLOCKS_WAKE_EN1_CLK_PERI_SPI1_BITS |
												CLOCKS_WAKE_EN1_CLK_SYS_SPI0_BITS |
												CLOCKS_WAKE_EN1_CLK_PERI_SPI0_BITS |
												CLOCKS_WAKE_EN1_CLK_SYS_SRAM0_BITS |
												CLOCKS_WAKE_EN1_CLK_SYS_SRAM1_BITS |
												CLOCKS_WAKE_EN1_CLK_SYS_SRAM2_BITS |
												CLOCKS_WAKE_EN1_CLK_SYS_SRAM3_BITS |
												CLOCKS_WAKE_EN1_CLK_SYS_SRAM4_BITS |
												CLOCKS_WAKE_EN1_CLK_SYS_SRAM5_BITS |
												CLOCKS_WAKE_EN1_CLK_SYS_SRAM6_BITS |
												CLOCKS_WAKE_EN1_CLK_SYS_SRAM7_BITS |
												CLOCKS_WAKE_EN1_CLK_SYS_SRAM8_BITS |
												CLOCKS_WAKE_EN1_CLK_SYS_SRAM9_BITS |
												CLOCKS_WAKE_EN1_CLK_SYS_TIMER0_BITS |
												CLOCKS_WAKE_EN1_CLK_REF_TICKS_BITS | // Need the TICKS sub-system for REF and SYS to restart all ticks when changing the clock
												CLOCKS_WAKE_EN1_CLK_SYS_TICKS_BITS |
												CLOCKS_WAKE_EN1_CLK_SYS_XIP_BITS |
												CLOCKS_WAKE_EN1_CLK_SYS_XOSC_BITS;
}

void leverage_turn_off_clocks() {
	clock_hw_t *clock_hw = &clocks_hw->clk[clk_usb];
	hw_clear_bits(&clock_hw->ctrl, CLOCKS_CLK_USB_CTRL_ENABLE_BITS);
	clock_hw = &clocks_hw->clk[clk_adc];
	hw_clear_bits(&clock_hw->ctrl, CLOCKS_CLK_ADC_CTRL_ENABLE_BITS);
	clock_hw = &clocks_hw->clk[clk_hstx];
	hw_clear_bits(&clock_hw->ctrl, CLOCKS_CLK_HSTX_CTRL_ENABLE_BITS);
}

int main() {
	//assign_clk_src();
	//deactivate_pll();
	//deactivate_xosc();
	//clock_stop(clk_adc);
	//clock_stop(clk_usb);
	//clock_stop(clk_hstx);
	//rosc_disable();
	//setup_default_uart();
	//leverage_reduced_wake_en_gating();
	//leverage_turn_off_clocks();
	//leverage_pull_down_gpios();
	//sleep_ms(500);
	//leverage_clock_source_lposc();
	//vreg_set_voltage(9);
	
	//sleep_run_from_dormant_source(DORMANT_SOURCE_LPOSC);
	
	stdio_init_all(); // pins + init things such as uart and usb 
	sleep_ms(1000);
	printf("testwoweifjiowefj\n");
	pico2_spi_init_default(115200);
	sx1262_pico2_init(&dio_gpio_callback);
	bme680_pico2_init();
	uint16_t irq_mask  = 0b0000001000000001; // Activate TxDone and timeout IRQ (8.5 IRQ Handling)
	sx1262_lora_init(&sx1262_connection, irq_mask);
	wait_sx1262_busy();
	SX1262_GET_STATUS(sx126x_set_pa_cfg(&sx1262_connection, &pa_cfg), "sx126x_set_pa_cfg");
	wait_sx1262_busy();
	SX1262_GET_STATUS(sx126x_set_tx_params(&sx1262_connection, tx_params.power, tx_params.ramp_time), "sx126x_set_tx_params");
	
	
	//sleep_us((int)(1000000*TIME_RATE));
	uint32_t timeout_ms = 1000;
	uint32_t measurements[4] = {0x0000, 0x0000, 0x0000, 0x0000};
	uint16_t nb_iterations = NB_ITERATIONS;
	absolute_time_t t = get_absolute_time();
	int64_t ms = to_ms_since_boot(t);
	absolute_time_t new_t = get_absolute_time();
	int64_t new_ms = to_ms_since_boot(t);
	//printf("Time since boot: %lld ms\n", ms);
	//led_blink(4);
	uint16_t nb_iter = 7000;
	uint16_t sleep_duration = 1000;
	for (int i = 0; i < nb_iter; i++) {
		//printf("Reading sensor data...\n");
		trigger_bme680_msrmt(measurements);
		sleep_us((int)(50000*TIME_RATE));

		//printf("time: %d\n", ts->tv_sec);
		// Write buffer
		uint8_t offset = 0; // Where to write in data buffer (value between 0 and 256). The data buffer is shared for tx and rx
		uint8_t buffer[PAYLOAD_LENGTH] = {
			measurements[0], measurements[0] >> 8, measurements[0] >> 16, measurements[0] >> 24, // temperature
			//measurements[1], measurements[1] >> 8, measurements[1] >> 16, measurements[1] >> 24, // humidity
			//measurements[2], measurements[2] >> 8, measurements[2] >> 16, measurements[2] >> 24, // pressure
			//measurements[3], measurements[3] >> 8, measurements[3] >> 16, measurements[3] >> 24, // gas
		};
		wait_sx1262_busy();
		sx126x_write_buffer(&sx1262_connection, offset, buffer, PAYLOAD_LENGTH);
		
		// Set radio to TX mode
		//printf("Sending data...\n");
		//new_t = get_absolute_time();
		//new_ms = to_ms_since_boot(new_t);
		//printf("Temperature to send: %d (in %lld ms)\n", measurements[0], new_ms - ms);
		//printf("new_ms, ms: %lld, %lld\n", new_ms,ms);
		//printf("Time since last send: %lld\n", new_ms - ms);
		//t = get_absolute_time();
		//ms = to_ms_since_boot(t);
		//printf("Humidity to send: %d\n", measurements[1]);
		//printf("Pressure to send: %d\n", measurements[2]);
		//printf("Gas resistance to send: %d\n", measurements[3]);
		wait_sx1262_busy();
		printf("sending...\n");
		sx126x_set_tx(&sx1262_connection, timeout_ms);
		printf("sending done\n");
		
		//sleep_ms((int)(SLEEP_RATE*sleep_duration));
		//if (i % 10 == 0) {
		//	led_blink(2);
		//	sleep_duration -= 100;
		//}
		////while(!tx_done) {tight_loop_contents();}
		////led_blink(2);
		////sleep_us((int)(1000000*TIME_RATE));
		//
		//struct timespec ts;
		//aon_timer_get_time(&ts);
		//////ts.tv_sec += 0;
		//ts.tv_nsec += (int)(SLEEP_RATE*1000000000);
		//go_dormant_until(&ts, &sleep_callback);
		//led_blink(6);
	}
	while(1) {printf("idle..."); sleep_us((int)(1000000*TIME_RATE));} // Do not exit else cannot reboot using picotool
	return 0;
}

