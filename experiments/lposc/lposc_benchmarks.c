#include "pico/stdlib.h"
#include "pico/sleep.h"
#include <pico/time.h>
#include <stdio.h>
#include "hardware/watchdog.h"
#include "hardware/clocks.h"
#include "hardware/powman.h"
#include "hardware/pll.h"
#include "hardware/xosc.h"
#include "hardware/vreg.h"
#include <pico/rand.h>
#include <stdlib.h>
#include "experiments.h"
// For wfi
#include "hardware/sync.h"
#include "pico/runtime_init.h"

#define VALIDATION_RUN 1
#define TARGET_VOLTAGE VREG_VOLTAGE_0_75
#define TARGET_LPOSC_TRIM 0x3f0
#define EXPE_NUM_OFFSET 25

const int expe_pin = 11;
const uint32_t RESET_VAL = 0xDEADBEEF; 
float TIME_RATE = 1;
volatile uint32_t iteration_num = 0;

int main() {
	// Inspired from https://github.com/peterharperuk/pico-examples/commit/7dccd00d15ded4ddf961f44fdcd1f11a9d8c8be1
	// Set pin to toggle experiments (also puts it to low)
	sleep_ms(100); // For unknown reason, not sleeping here make firmware upload using SWD to fail
	gpio_init(expe_pin);
	gpio_set_dir(expe_pin, GPIO_OUT);
	
	#if VALIDATION_RUN
	if(watchdog_hw->scratch[0] != RESET_VAL) {
		watchdog_hw->scratch[1] = 0;
	}
	#else
	if(watchdog_hw->scratch[0] != RESET_VAL) {
		// Leave 10sec window to unplug the SWD before resetting the board (required because the SWD sub-system doesn't deactivate automatically once SWD is unplugged (3.5.1. of datasheet))
		sleep_ms(10000); 
		// Scratch values survive between reboots between reboots
		watchdog_hw->scratch[0] = RESET_VAL;
		watchdog_reboot(0, 0, 0);
	}
	pull_down_gpios();
	turn_off_clocks();
	disable_usb();
	#endif

	// Dormant source: LPOSC
	leverage_clock_source_lposc();
	#if !VALIDATION_RUN
	clock_stop(clk_adc);
	clock_stop(clk_usb);
	clock_stop(clk_hstx);
	setup_default_uart();
	stdio_flush();
	#endif
	processor_deep_sleep();
	vreg_disable_voltage_limit();
	powman_clear_bits(&powman_hw->bod, 0x000001f1);
	vreg_set_voltage(TARGET_VOLTAGE); // Change value
	xosc_init();
	clock_configure_undivided(clk_ref, CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC, 0, XOSC_HZ);
	restart_all_ticks();
	
	// Setup voltage and trimming
	powman_clear_bits(&powman_hw->lposc, POWMAN_LPOSC_TRIM_BITS);
	powman_set_bits(&powman_hw->lposc, POWMAN_LPOSC_TRIM_BITS & TARGET_LPOSC_TRIM);
	uint lposc_freq = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_LPOSC_CLKSRC)*KHZ;
	TIME_RATE = ((float)lposc_freq)/((float)1*MHZ);
	clock_configure_undivided(clk_ref, CLOCKS_CLK_REF_CTRL_SRC_VALUE_LPOSC_CLKSRC, 0, lposc_freq);
	restart_all_ticks();
	xosc_disable();
	
	clock_set_reported_hz(clk_ref, lposc_freq);
	clock_set_reported_hz(clk_sys, lposc_freq);
	
	gpio_put(expe_pin, 1);
	uint benchmark_result_prime = benchmark_prime(200); // Uses one CPU core
	gpio_put(expe_pin, 0);
	sleep_us((int)(100000*TIME_RATE));
	gpio_put(expe_pin, 1);
	uint benchmark_result_multicores = benchmark_prime_multicores(200); // Uses both cores
	gpio_put(expe_pin, 0);
	sleep_us((int)(100000*TIME_RATE));
	gpio_put(expe_pin, 1);
	uint8_t benchmark_result_mat_mul = benchmark_mat_mul(72); // Uses RAM
	gpio_put(expe_pin, 0);
	sleep_us((int)(100000*TIME_RATE));
	gpio_put(expe_pin, 1);
	uint8_t benchmark_result_mat_mul_float = benchmark_mat_mul_float(72); // Uses float co-processor
	gpio_put(expe_pin, 0);
	gpio_put(expe_pin, 1);
	uint8_t benchmark_result_mat_mul_double = benchmark_mat_mul_double(36); // Uses double co-processor
	gpio_put(expe_pin, 0);
	
	vreg_set_voltage(VREG_VOLTAGE_DEFAULT); // Change value
	
	#if VALIDATION_RUN
	xosc_init();
	clock_configure_undivided(clk_ref, CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC, 0, XOSC_HZ);
	pll_deinit(pll_sys);
	pll_deinit(pll_usb);
	restart_all_ticks();
	pll_init(pll_sys, PLL_SYS_REFDIV, PLL_SYS_VCO_FREQ_HZ, PLL_SYS_POSTDIV1, PLL_SYS_POSTDIV2);
	pll_init(pll_usb, PLL_USB_REFDIV, PLL_USB_VCO_FREQ_HZ, PLL_USB_POSTDIV1, PLL_USB_POSTDIV2);
	clock_configure_undivided(clk_sys, CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX, CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS, SYS_CLK_HZ);
	clock_configure_undivided(clk_peri,
									0,
									CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS,
									SYS_CLK_HZ);
	
	stdio_init_all();
	sleep_ms(1000);
	
	uint iteration_num = watchdog_hw->scratch[1];
	printf("%d,%d,%d,%d,,,\n", iteration_num, EXPE_NUM_OFFSET, benchmark_result_prime, lposc_freq);
	printf("%d,%d,%d,%d,,,\n", iteration_num, EXPE_NUM_OFFSET+1, benchmark_result_multicores, lposc_freq);
	printf("%d,%d,%d,%d,,,\n", iteration_num, EXPE_NUM_OFFSET+2, benchmark_result_mat_mul, lposc_freq);
	printf("%d,%d,%d,%d,,,\n", iteration_num, EXPE_NUM_OFFSET+3, benchmark_result_mat_mul_float, lposc_freq);
	printf("%d,%d,%d,%d,,,\n", iteration_num, EXPE_NUM_OFFSET+4, benchmark_result_mat_mul_double, lposc_freq);
	watchdog_hw->scratch[1] += 1;
	#endif
	
	// End of iteration, reset the board
	watchdog_hw->scratch[0] = RESET_VAL;
	watchdog_reboot(0, 0, 0);
	return 0; // Should never reach here
}
