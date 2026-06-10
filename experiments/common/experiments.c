#include "experiments.h"
#include "target_configuration.h"
#include "pico/stdlib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pico/multicore.h"
#include "hardware/clocks.h"
#include "hardware/structs/usb.h"
#include "hardware/ticks.h"
#include "hardware/pll.h"
#include "hardware/xosc.h"
#include "hardware/rosc.h"
#include "hardware/watchdog.h"
#include "hardware/vreg.h"
#include "hardware/powman.h"

#define NB_BENCHMARKS 6

// Benchmark sizes
#define BENCH_NOOP_SIZE 10
#define BENCH_PRIME_SIZE 5000
#define BENCH_PRIME_SIZE_LPOSC 200
#define BENCH_MAT_SIZE 43000
#define NB_ITERATIONS_MAT_MUL 10
#define NB_ITERATIONS_MAT_MUL_LPOSC 1

// Benchmark correct results
#define CORRECT_PRIME 669
#define CORRECT_PRIME_LPOSC 46
#define CORRECT_MAT_MUL 4294967295
#define CORRECT_MAT_MUL_FLOAT_UPPER 0.525 
#define CORRECT_MAT_MUL_FLOAT_LOWER 0.524 
#define CORRECT_MAT_MUL_DOUBLE 0.5241578750190518665164063349948264658451080322265625

// Constants
const uint US = 1000000;
const uint32_t RESET_VAL = 0xDEADBEEF; 

const int expe_pin = 11;
float TIME_RATE = 1;

void iteration_init(uint phase, uint vreg_expe) {
	sleep_ms(100); // For unknown reason, not sleeping here sometimes makes firmware upload using SWD to fail
	
	if(watchdog_hw->scratch[0] != RESET_VAL) {
		watchdog_hw->scratch[1] = 0; // Iteration num
	}
	
	if(phase==1) {
		if(watchdog_hw->scratch[0] != RESET_VAL) {
			// Leave 10sec window to unplug the SWD before resetting the board (required because the SWD sub-system doesn't deactivate automatically once SWD is unplugged (3.5.1. of datasheet))
			sleep_ms(10000); 
			// Scratch values survive between reboots between reboots
			watchdog_hw->scratch[0] = RESET_VAL;
			watchdog_reboot(0, 0, 0);
		}
		//pull_down_gpios();
		//turn_off_clocks();
		//disable_usb();
	}
	if(vreg_expe != VREG_VOLTAGE_DEFAULT) {
		vreg_disable_voltage_limit();
		powman_clear_bits(&powman_hw->bod, 0x000001f1);
	}
	// Set GPIO pin to advertise experiments start and end, and puts it to low
	gpio_init(expe_pin);
	gpio_set_dir(expe_pin, GPIO_OUT);
}

void iteration_end(uint phase, char* buffer) {
	
	if(phase==0) {
		// Reinit the PLLs to print results
		xosc_init();
		clock_configure_undivided(clk_ref, CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC, 0, XOSC_HZ);
		clock_configure_undivided(clk_sys, CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX, CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_XOSC_CLKSRC, XOSC_HZ);
		sleep_us((int)(1*US*TIME_RATE));
		vreg_set_voltage(VREG_VOLTAGE_DEFAULT);
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
		printf("%s", buffer);
	}
	
	watchdog_hw->scratch[1] += 1; // Next experiment

	// End of iteration, reset the board
	watchdog_hw->scratch[0] = RESET_VAL;
	sleep_us((int)(10*US*TIME_RATE));
	vreg_set_voltage(VREG_VOLTAGE_DEFAULT);
	sleep_us((int)(20*US*TIME_RATE));
	watchdog_reboot(0, 0, 0);
}

uint8_t benchmark_noop(uint benchmark_size) {
	sleep_us((int)(benchmark_size*US*TIME_RATE));
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

uint8_t benchmark_prime(uint benchmark_size) {
	volatile uint cpt = compute_primes(2, benchmark_size);
	uint8_t correct = 1;
	if(benchmark_size == 200 && cpt != CORRECT_PRIME_LPOSC) {
		correct = 0;
	}
	if(benchmark_size == 5000 && cpt != CORRECT_PRIME) {
		correct = 0;
	}
	return correct;
}

void compute_primes_core1() {
	uint32_t start = multicore_fifo_pop_blocking();
	uint32_t end = multicore_fifo_pop_blocking();
	uint cpt = compute_primes(start, end);
	multicore_fifo_push_blocking(cpt);
}

uint8_t benchmark_prime_multicores(uint benchmark_size) {
	multicore_reset_core1();
	multicore_launch_core1(compute_primes_core1);
	multicore_fifo_push_blocking(benchmark_size/2);
	multicore_fifo_push_blocking(benchmark_size);
	uint cpt_core0 = compute_primes(2, benchmark_size/2);
	uint cpt_core1 = multicore_fifo_pop_blocking();
	uint8_t correct = 1;
	if(benchmark_size == 200 && cpt_core0 + cpt_core1 != CORRECT_PRIME_LPOSC) {
		correct = 0;
	}
	if(benchmark_size == 5000 && cpt_core0 + cpt_core1 != CORRECT_PRIME) {
		correct = 0;
	}
	return correct;
}

uint8_t benchmark_mat_mul(uint benchmark_size, uint nb_iteration_mat_mul) {
	uint32_t A_value = 1UL<<16;
	uint32_t B_value = 1UL<<16;
	volatile uint8_t correct = 1;
	for (int k = 0; k < nb_iteration_mat_mul; k++) {
		// Three 32-bits matrixes of 72 elements account for 486 kB, which should account for all 8 memory banks in SRAM0 and SRAM1
		uint32_t *A = malloc(sizeof(uint32_t)*benchmark_size);
		uint32_t *B = malloc(sizeof(uint32_t)*benchmark_size);
		uint32_t *C = malloc(sizeof(uint32_t)*benchmark_size);
		for (int i = 0; i < benchmark_size; i++) {
			A[i] = A_value;
			B[i] = B_value;
		}
		for (int i = 0; i < benchmark_size; i++) {
			C[i] = A[i] * B[i] - 1;
		}
		// Verification
		for (int i = 0; i < benchmark_size; i++) {
			if(C[i] != CORRECT_MAT_MUL) {
				correct = 0;
			}
		}
		free(A);
		free(B);
		free(C);
	}
	return correct;
}

uint8_t benchmark_mat_mul_float(uint benchmark_size, uint nb_iteration_mat_mul) {
	float A_value = 1.23456789;
	float B_value = 1.23456789;
	volatile uint8_t correct = 1;
	for (int k = 0; k < nb_iteration_mat_mul; k++) {
		// Three 32-bits matrixes of 72 elements account for 486 kB, which should account for all 8 memory banks in SRAM0 and SRAM1
		float *A = malloc(sizeof(float)*benchmark_size);
		float *B = malloc(sizeof(float)*benchmark_size);
		float *C = malloc(sizeof(float)*benchmark_size);
		for (int i = 0; i < benchmark_size; i++) {
			A[i] = A_value;
			B[i] = B_value;
		}
		for (int i = 0; i < benchmark_size; i++) {
			C[i] = A[i] * B[i] - 1;
		}
		// Verification
		for (int i = 0; i < benchmark_size; i++) {
			if(C[i] > CORRECT_MAT_MUL_FLOAT_UPPER || C[i] < CORRECT_MAT_MUL_FLOAT_LOWER) {
				correct = 0;
			}
		}
		free(A);
		free(B);
		free(C);
	}
	return correct;
}

uint8_t benchmark_mat_mul_double(uint benchmark_size, uint nb_iteration_mat_mul) {
	double A_value = 1.23456789;
	double B_value = 1.23456789;
	volatile uint8_t correct = 1;
	for (int k = 0; k < nb_iteration_mat_mul; k++) {
		// Three 32-bits matrixes of 72 elements account for 486 kB, which should account for all 8 memory banks in SRAM0 and SRAM1
		double *A = malloc(sizeof(double)*benchmark_size);
		double *B = malloc(sizeof(double)*benchmark_size);
		double *C = malloc(sizeof(double)*benchmark_size);
		for (int i = 0; i < benchmark_size; i++) {
			A[i] = A_value;
			B[i] = B_value;
		}
		for (int i = 0; i < benchmark_size; i++) {
			C[i] = A[i] * B[i] - 1;
		}
		// Verification
		for (int i = 0; i < benchmark_size; i++) {
			if((double)C[i] != CORRECT_MAT_MUL_DOUBLE) {
				correct = 0;
			}
		}
		free(A);
		free(B);
		free(C);
	}
	return correct;
}

uint8_t execute_benchmarks(bool clock_source_lposc, uint8_t benchmarks_to_run, struct bench_sizes bench_sizes) {
	// Sleep 10 seconds before starting benchmarks
	sleep_us((int)(10*US*TIME_RATE));
	uint bench_prime_size = clock_source_lposc ? BENCH_PRIME_SIZE_LPOSC : BENCH_PRIME_SIZE;
	uint nb_iteration_mat_mul = clock_source_lposc ? NB_ITERATIONS_MAT_MUL_LPOSC : NB_ITERATIONS_MAT_MUL;
	uint noop_size = bench_sizes.noop;
	uint prime_size = clock_source_lposc?bench_sizes.prime_lposc:bench_sizes.prime;
	uint mat_mul_iters = clock_source_lposc?bench_sizes.mat_mul_iters_lposc:bench_sizes.mat_mul_iters;
	uint8_t results = 1; // noop result is always 1
	
	// Noop
	if(benchmarks_to_run & 0b000001) {
		gpio_put(expe_pin, 1);
		benchmark_noop(noop_size); // Uses one CPU core
		gpio_put(expe_pin, 0);
		sleep_us((int)(100000*TIME_RATE));
	}
	
	// Prime
	if(benchmarks_to_run & 0b000010) {
		gpio_put(expe_pin, 1);
		results |= benchmark_prime(prime_size) << 1; // Uses one CPU core
		gpio_put(expe_pin, 0);
		sleep_us((int)(100000*TIME_RATE));
	}
	
	// Prime multicores
	if(benchmarks_to_run & 0b000100) {
		gpio_put(expe_pin, 1);
		results |= benchmark_prime_multicores(prime_size) << 2; // Uses both cores
		gpio_put(expe_pin, 0);
		sleep_us((int)(100000*TIME_RATE));
	}
	
	// Mat mul int
	if(benchmarks_to_run & 0b001000) {
		gpio_put(expe_pin, 1);
		results |= benchmark_mat_mul(BENCH_MAT_SIZE, mat_mul_iters) << 3; // Uses RAM
		gpio_put(expe_pin, 0);
		sleep_us((int)(100000*TIME_RATE));
	}
	
	// Mat mul float
	if(benchmarks_to_run & 0b010000) {
		gpio_put(expe_pin, 1);
		results |= benchmark_mat_mul_float(BENCH_MAT_SIZE, mat_mul_iters) << 4; // Uses float co-processor
		gpio_put(expe_pin, 0);
		sleep_us((int)(100000*TIME_RATE));
	}
	
	// Mat mul double
	if(benchmarks_to_run & 0b100000) {
		gpio_put(expe_pin, 1);
		results |= benchmark_mat_mul_double(BENCH_MAT_SIZE/2, mat_mul_iters) << 5; // Uses double co-processor
		gpio_put(expe_pin, 0);
	}
	sleep_us((int)(1000000*TIME_RATE));
	return results;
}

void led_blink(uint count) {
	gpio_init(PICO_DEFAULT_LED_PIN);
	gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
	for (int i = 0; i < count; i++) {
		gpio_put(PICO_DEFAULT_LED_PIN, 1);
		sleep_us((int)(250000*TIME_RATE));
		gpio_put(PICO_DEFAULT_LED_PIN, 0);
		sleep_us((int)(250000*TIME_RATE));
	}
}

void pull_down_gpios() {
	for (int gpio = 0; gpio < NUM_BANK0_GPIOS; gpio++) {
		gpio_set_dir(gpio, 0);
		gpio_set_function(gpio, GPIO_FUNC_SIO);
		if(gpio > NUM_BANK0_GPIOS - NUM_ADC_CHANNELS) {
			gpio_disable_pulls(gpio);
			gpio_set_input_enabled(gpio, false);
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

void processor_deep_sleep(void) {
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

uint bench_size_by_bench_num(struct bench_sizes bench_sizes, uint bench_num, bool is_lposc) {
	if(bench_num == 0) return bench_sizes.noop;
	if(bench_num == 1 || bench_num == 2) return is_lposc?bench_sizes.prime_lposc:bench_sizes.prime;
	return is_lposc?bench_sizes.mat_mul_iters_lposc:bench_sizes.mat_mul_iters;
}

void print_configurations_csv(char* buffer, void* configurations, uint nb_expes, uint8_t benchmarks_to_run, struct bench_sizes bench_sizes, uint b_num, bool is_lposc, uint clock_freq) {
	const char* benchmark_names[] = {"noop", "prime", "prime_multicores", "mat_mul", "mat_mul_float", "mat_mul_double"};
	const char* vreg_strings[] = {"0.55V", "0.60V", "0.65V", "0.70V", "0.75V", "0.80V", "0.85V", "0.90V", "0.95V", "1.00V", "1.05V", "1.10V", "1.15V", "1.20V", "1.25V"};
	const char* clk_name[] = {"PLL", "XOSC", "ROSC", "LPOSC"};
	const struct config conf = *(const struct config*) configurations;
	for (int bench_num = 0; bench_num < NB_BENCHMARKS; bench_num++) {
		if(benchmarks_to_run & (1 << bench_num)) {
			uint bench_size = bench_size_by_bench_num(bench_sizes, bench_num, is_lposc);
			sprintf(buffer+strlen(buffer), "%s,%d,%d,%d,%d,%d,%d,%d,0x%.3x,%s,%b,%s,%d,%d,%d\n", clk_name[conf.clock_source], conf.pll_vco_freq, conf.pll_div1, conf.pll_div2, conf.rosc_div, conf.rosc_range, conf.rosc_drive_freqa, conf.rosc_drive_freqb, conf.lposc_trim, vreg_strings[conf.vreg_output], conf.set_as_ref, benchmark_names[bench_num], bench_size, b_num, clock_freq);
		}
	}
}
