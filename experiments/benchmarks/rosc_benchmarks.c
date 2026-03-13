#include "pico/stdlib.h"
#include "pico/sleep.h"
#include <pico/time.h>
#include <stdio.h>
#include "hardware/watchdog.h"
#include "hardware/clocks.h"
#include "hardware/structs/usb.h"
#include "hardware/powman.h"
#include "hardware/pll.h"
#include "hardware/xosc.h"
#include "hardware/ticks.h"
#include "hardware/vreg.h"
#include "pico/multicore.h"
#include <pico/rand.h>
#include <stdlib.h>
// For wfi
#include "hardware/sync.h"
#include "pico/runtime_init.h"

#define DEBUG_RUN 0
#if DEBUG_RUN
#define debug_printf(format, args...) printf(format, ## args)
#else
#define debug_printf(...)
#endif
#define VALIDATION_RUN 0
#if VALIDATION_RUN
#define validation_printf(format, args...) printf(format, ## args)
#else
#define validation_printf(...)
#endif

const int expe_pin = 11;
const uint32_t RESET_VAL = 0xDEADBEEF; 
float TIME_RATE = 1;
const uint NB_ITERATIONS_MAT_MUL = 1000;

void pull_down_gpios() {
	const uint used_gpios[] = {expe_pin};
	size_t nb_gpios = sizeof(used_gpios)/sizeof(used_gpios[0]);
	for (int gpio = 0; gpio < NUM_BANK0_GPIOS; gpio++) {
		if(!expe_pin) {
			gpio_set_dir(gpio, 0);
			gpio_set_function(gpio, GPIO_FUNC_SIO);
			if(gpio > NUM_BANK0_GPIOS - NUM_ADC_CHANNELS) {
				gpio_disable_pulls(gpio);
				gpio_set_input_enabled(gpio, false);
			}
		}
	}
}

void turn_off_clocks() {
	clock_hw_t *clock_hw = &clocks_hw->clk[clk_usb];
	hw_clear_bits(&clock_hw->ctrl, CLOCKS_CLK_USB_CTRL_ENABLE_BITS);
	clock_hw = &clocks_hw->clk[clk_adc];
	hw_clear_bits(&clock_hw->ctrl, CLOCKS_CLK_ADC_CTRL_ENABLE_BITS);
	clock_hw = &clocks_hw->clk[clk_hstx];
	hw_clear_bits(&clock_hw->ctrl, CLOCKS_CLK_HSTX_CTRL_ENABLE_BITS);
}

static void disable_usb() {
    usb_hw->phy_direct = USB_USBPHY_DIRECT_TX_PD_BITS | USB_USBPHY_DIRECT_RX_PD_BITS | USB_USBPHY_DIRECT_DM_PULLDN_EN_BITS | USB_USBPHY_DIRECT_DP_PULLDN_EN_BITS;
    
    usb_hw->phy_direct_override = USB_USBPHY_DIRECT_RX_DM_BITS | USB_USBPHY_DIRECT_RX_DP_BITS |          USB_USBPHY_DIRECT_RX_DD_BITS |
        USB_USBPHY_DIRECT_OVERRIDE_TX_DIFFMODE_OVERRIDE_EN_BITS | USB_USBPHY_DIRECT_OVERRIDE_DM_PULLUP_OVERRIDE_EN_BITS | USB_USBPHY_DIRECT_OVERRIDE_TX_FSSLEW_OVERRIDE_EN_BITS |
        USB_USBPHY_DIRECT_OVERRIDE_TX_PD_OVERRIDE_EN_BITS | USB_USBPHY_DIRECT_OVERRIDE_RX_PD_OVERRIDE_EN_BITS | USB_USBPHY_DIRECT_OVERRIDE_TX_DM_OVERRIDE_EN_BITS |
        USB_USBPHY_DIRECT_OVERRIDE_TX_DP_OVERRIDE_EN_BITS | USB_USBPHY_DIRECT_OVERRIDE_TX_DM_OE_OVERRIDE_EN_BITS | USB_USBPHY_DIRECT_OVERRIDE_TX_DP_OE_OVERRIDE_EN_BITS |
        USB_USBPHY_DIRECT_OVERRIDE_DM_PULLDN_EN_OVERRIDE_EN_BITS | USB_USBPHY_DIRECT_OVERRIDE_DP_PULLDN_EN_OVERRIDE_EN_BITS | USB_USBPHY_DIRECT_OVERRIDE_DP_PULLUP_EN_OVERRIDE_EN_BITS |
        USB_USBPHY_DIRECT_OVERRIDE_DM_PULLUP_HISEL_OVERRIDE_EN_BITS | USB_USBPHY_DIRECT_OVERRIDE_DP_PULLUP_HISEL_OVERRIDE_EN_BITS;
}

static void sleep_callback(void) {}

static void processor_deep_sleep(void) {
		// From pico-extras
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
	// lposc frequency varies according to voltage and temperature. Its frequency needs to be counted before switching clock ref
	// XOSC needs to be on the clk_ref to accurately count the frequency
	xosc_init();
	clock_configure_undivided(clk_ref, CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC, 0, XOSC_HZ);
	restart_all_ticks();
	
	uint lposc_freq = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_LPOSC_CLKSRC)*KHZ;
	clock_configure_undivided(clk_ref, CLOCKS_CLK_REF_CTRL_SRC_VALUE_LPOSC_CLKSRC, 0, lposc_freq);
	clock_configure_undivided(clk_sys, CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLK_REF, 0, lposc_freq);
	clock_configure(clk_peri,
									0,
									CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS,
									lposc_freq,
									lposc_freq);
	TIME_RATE = ((float)lposc_freq)/((float)1*MHZ); // LPOSC isn't fast enough to generate the 1us tick (hardwired value). The TIME_RATE divides any active wait to account for this slowness
	restart_all_ticks();
	pll_deinit(pll_sys);
	pll_deinit(pll_usb);
	rosc_disable();
	xosc_disable();
}

uint32_t benchmark_mat_mul(uint benchmark_size) {
	srand(0);
	volatile uint32_t sum_c_res = 0;
	for (int k = 0; k < NB_ITERATIONS_MAT_MUL; k++) {
		// Three 32-bits matrixes of 72 elements account for 486 kB, which should account for all 8 memory banks in SRAM0 and SRAM1
		uint32_t *A = malloc(sizeof(uint32_t)*benchmark_size*benchmark_size);
		uint32_t *B = malloc(sizeof(uint32_t)*benchmark_size*benchmark_size);
		uint32_t *C = malloc(sizeof(uint32_t)*benchmark_size*benchmark_size);
		for (int i = 0; i < benchmark_size*benchmark_size; i++) {
			A[i] = rand(); // RAND_MAX = 32767 by default (2<<15)
			B[i] = rand();
		}
		for (int i = 0; i < benchmark_size*benchmark_size; i++) {
			C[i] = A[i] * B[i];
		}
		for (int i = 0; i < benchmark_size*benchmark_size; i++) {
			sum_c_res = (sum_c_res + C[i])%(2<<30);
		}
		free(A);
		free(B);
		free(C);
	}
	return sum_c_res;
}

uint benchmark_mat_mul_float(uint benchmark_size) {
	srand(0);
	volatile float sum_c_res = 0;
	for (int k = 0; k < NB_ITERATIONS_MAT_MUL; k++) {
		// Three 32-bits matrixes of 72 elements account for 486 kB, which should account for all 8 memory banks in SRAM0 and SRAM1
		float *A = malloc(sizeof(float)*benchmark_size*benchmark_size);
		float *B = malloc(sizeof(float)*benchmark_size*benchmark_size);
		float *C = malloc(sizeof(float)*benchmark_size*benchmark_size);
		for (int i = 0; i < benchmark_size*benchmark_size; i++) {
			A[i] = rand(); // RAND_MAX = 32767 by default (2<<15)
			B[i] = rand();
		}
		for (int i = 0; i < benchmark_size*benchmark_size; i++) {
			C[i] = A[i] * B[i] * 1.2345;
		}
		for (int i = 0; i < benchmark_size*benchmark_size; i++) {
			sum_c_res = sum_c_res + C[i];
		}
		free(A);
		free(B);
		free(C);
	}
	return sum_c_res;
}

uint benchmark_mat_mul_double(uint benchmark_size) {
	srand(0);
	volatile double sum_c_res = 0;
	for (int k = 0; k < NB_ITERATIONS_MAT_MUL; k++) {
		// Three 64-bits matrixes of 36 elements account for 442kB, which should account for all 8 memory banks in SRAM0 and SRAM1
		double *A = malloc(sizeof(double)*benchmark_size*benchmark_size);
		double *B = malloc(sizeof(double)*benchmark_size*benchmark_size);
		double *C = malloc(sizeof(double)*benchmark_size*benchmark_size);
		for (int i = 0; i < benchmark_size*benchmark_size; i++) {
			A[i] = rand(); // RAND_MAX = 32767 by default (2<<15)
			B[i] = rand();
		}
		for (int i = 0; i < benchmark_size*benchmark_size; i++) {
			C[i] = A[i] * B[i] * 1.2345;
		}
		for (int i = 0; i < benchmark_size*benchmark_size; i++) {
			sum_c_res = sum_c_res + C[i];
		}
		free(A);
		free(B);
		free(C);
	}
	return sum_c_res;
}

uint compute_primes(uint start, uint end) {
	volatile uint cpt = 0;
	uint is_prime = 1;
	for (int k = start; k < end; k++) {
		is_prime = 1;
		for (int i = 2; i	< k; i++) {
			if (k%i==0) {
				is_prime = 0;
			}	
		}
		if(is_prime == 1) {
			cpt += 1;
		}
	}
	return cpt;
}

uint benchmark_prime(uint benchmark_size) {
	volatile uint cpt = compute_primes(2, benchmark_size);
	return cpt;
}

void compute_primes_core1() {
	uint32_t start = multicore_fifo_pop_blocking();
	uint32_t end = multicore_fifo_pop_blocking();
	uint cpt = compute_primes(start, end);
	multicore_fifo_push_blocking(cpt);
}

uint benchmark_prime_multicores(uint benchmark_size) {
	multicore_reset_core1();
	multicore_launch_core1(compute_primes_core1);
	multicore_fifo_push_blocking(benchmark_size/2);
	multicore_fifo_push_blocking(benchmark_size);
	uint cpt_core0 = compute_primes(2, benchmark_size/2);
	uint cpt_core1 = multicore_fifo_pop_blocking();
	return cpt_core0 + cpt_core1;
}

void leverage_clock_source_rosc() {
	rosc_enable();
	xosc_init();
	clock_configure_undivided(clk_ref, CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC, 0, XOSC_HZ);
	restart_all_ticks();
	uint rosc_freq = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_ROSC_CLKSRC_PH);
	clock_configure_undivided(clk_ref, CLOCKS_CLK_REF_CTRL_SRC_VALUE_ROSC_CLKSRC_PH, 0, rosc_freq);
	clock_configure_undivided(clk_sys, CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX, CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_ROSC_CLKSRC, rosc_freq);
	//clock_configure(clk_peri, 0, CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS, rosc_freq, rosc_freq);
	restart_all_ticks();
	pll_deinit(pll_sys);
	pll_deinit(pll_usb);
	xosc_disable();
}

int main() {
	// Set on/off pin to toggle experiments (also puts the expe_pin to 0)
	// Inspired from https://github.com/peterharperuk/pico-examples/commit/7dccd00d15ded4ddf961f44fdcd1f11a9d8c8be1
	gpio_init(expe_pin);
	gpio_set_dir(expe_pin, GPIO_OUT);
	
	if(watchdog_hw->scratch[0] != RESET_VAL) {
		// Leave 10sec window to unplug the SWD before resetting the board (required because the SWD sub-system doesn't deactivate automatically once SWD is unplugged (3.5.1. of datasheet))
		debug_printf("Leaving 10s to unplug the swd");
		sleep_ms(10000); 
		// Scratch values survive between reboots between reboots
		watchdog_hw->scratch[0] = RESET_VAL;
		watchdog_reboot(0, 0, 0);
	}
	
	#if DEBUG_RUN
	stdio_init_all();
	sleep_ms(1000);
	debug_printf("Debug run\n");
	#else
	pull_down_gpios();
	turn_off_clocks();
	disable_usb();
	#endif

	leverage_clock_source_rosc();
	clock_stop(clk_adc);
	clock_stop(clk_usb);
	clock_stop(clk_hstx);
	setup_default_uart();
	stdio_flush();
	processor_deep_sleep();
	vreg_disable_voltage_limit();
	powman_clear_bits(&powman_hw->bod, 0x000001f1);
	vreg_set_voltage(VREG_VOLTAGE_0_75); // Change value
	xosc_init();
	clock_configure_undivided(clk_ref, CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC, 0, XOSC_HZ);
	restart_all_ticks();
	
	// Setup voltage and trimming
	rosc_set_div(1);
	rosc_set_range(1);
	rosc_set_freq(0);
	uint rosc_freq = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_ROSC_CLKSRC)*KHZ;
	clock_configure_undivided(clk_ref, CLOCKS_CLK_REF_CTRL_SRC_VALUE_ROSC_CLKSRC_PH, 0, rosc_freq);
	restart_all_ticks();
	xosc_disable();
	
	clock_set_reported_hz(clk_ref, rosc_freq);
	clock_set_reported_hz(clk_sys, rosc_freq);
	
	uint benchmark_result;
	gpio_put(expe_pin, 1);
	benchmark_result = benchmark_prime(10000); // Uses one CPU core
	gpio_put(expe_pin, 0);
	validation_printf(",10,%d,%d,,,", benchmark_result, lposc_freq);
	sleep_us((int)(1000000*TIME_RATE));
	gpio_put(expe_pin, 1);
	benchmark_result = benchmark_prime_multicores(10000); // Uses both cores
	gpio_put(expe_pin, 0);
	validation_printf(",11,%d,%d,,,", benchmark_result, lposc_freq);
	sleep_us((int)(1000000*TIME_RATE));
	gpio_put(expe_pin, 1);
	benchmark_result = benchmark_mat_mul(72); // Uses RAM
	gpio_put(expe_pin, 0);
	validation_printf(",12,%d,%d,,,", benchmark_result, lposc_freq);
	sleep_us((int)(1000000*TIME_RATE));
	gpio_put(expe_pin, 1);
	float benchmark_result_float = benchmark_mat_mul_float(72); // Uses float co-processor
	gpio_put(expe_pin, 0);
	validation_printf(",13,%.15f,%d,,,", benchmark_result_float, lposc_freq);
	sleep_us((int)(1000000*TIME_RATE));
	gpio_put(expe_pin, 1);
	double benchmark_result_double = benchmark_mat_mul_double(36); // Uses double co-processor
	gpio_put(expe_pin, 0);
	validation_printf(",14,%.15f,%d,,,", benchmark_result_double, lposc_freq);
	sleep_us((int)(1000000*TIME_RATE));
	
	vreg_set_voltage(VREG_VOLTAGE_DEFAULT); // Change value
	// End of iteration, reset the board
	watchdog_hw->scratch[0] = RESET_VAL;
	watchdog_reboot(0, 0, 0);
	return 0; // Should never reach here
}
