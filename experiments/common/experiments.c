#include "experiments.h"
#include "pico/stdlib.h"
#include <stdlib.h>
#include <stdio.h>
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

const uint US = 1000000;
const uint32_t RESET_VAL = 0xDEADBEEF; 
const int expe_pin = 11;
extern float TIME_RATE;

#define CORRECT_PRIME_200 46
#define CORRECT_PRIME_5000 669
#define CORRECT_MAT_MUL 4294967295
#define CORRECT_MAT_MUL_FLOAT_UPPER 0.525 
#define CORRECT_MAT_MUL_FLOAT_LOWER 0.524 
#define CORRECT_MAT_MUL_DOUBLE 0.5241578750190518665164063349948264658451080322265625

void iteration_init() {
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
	vreg_disable_voltage_limit();
	powman_clear_bits(&powman_hw->bod, 0x000001f1);
}

void iteration_end(char** strings_buffer, uint index_buff) {
	vreg_set_voltage(VREG_VOLTAGE_DEFAULT);
	
	#if VALIDATION_RUN
	// Reinit the PLLs to print results
	xosc_init();
	clock_configure_undivided(clk_ref, CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC, 0, XOSC_HZ);
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
	
	for (int i = 0; i < index_buff; i++) {
		printf(strings_buffer[i]);
	}
	
	watchdog_hw->scratch[1] += 1;
	#endif
	
	// End of iteration, reset the board
	watchdog_hw->scratch[0] = RESET_VAL;
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
	if(benchmark_size == 200 && cpt != CORRECT_PRIME_200) {
		correct = 0;
	}
	if(benchmark_size == 5000 && cpt != CORRECT_PRIME_5000) {
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
	if(benchmark_size == 200 && cpt_core0 + cpt_core1 != CORRECT_PRIME_200) {
		correct = 0;
	}
	if(benchmark_size == 5000 && cpt_core0 + cpt_core1 != CORRECT_PRIME_5000) {
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
		uint32_t *A = malloc(sizeof(uint32_t)*benchmark_size*benchmark_size);
		uint32_t *B = malloc(sizeof(uint32_t)*benchmark_size*benchmark_size);
		uint32_t *C = malloc(sizeof(uint32_t)*benchmark_size*benchmark_size);
		for (int i = 0; i < benchmark_size*benchmark_size; i++) {
			A[i] = A_value;
			B[i] = B_value;
		}
		for (int i = 0; i < benchmark_size*benchmark_size; i++) {
			C[i] = A[i] * B[i] - 1;
		}
		// Verification
		for (int i = 0; i < benchmark_size*benchmark_size; i++) {
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
		float *A = malloc(sizeof(float)*benchmark_size*benchmark_size);
		float *B = malloc(sizeof(float)*benchmark_size*benchmark_size);
		float *C = malloc(sizeof(float)*benchmark_size*benchmark_size);
		for (int i = 0; i < benchmark_size*benchmark_size; i++) {
			A[i] = A_value;
			B[i] = B_value;
		}
		for (int i = 0; i < benchmark_size*benchmark_size; i++) {
			C[i] = A[i] * B[i] - 1;
		}
		// Verification
		for (int i = 0; i < benchmark_size*benchmark_size; i++) {
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
		double *A = malloc(sizeof(double)*benchmark_size*benchmark_size);
		double *B = malloc(sizeof(double)*benchmark_size*benchmark_size);
		double *C = malloc(sizeof(double)*benchmark_size*benchmark_size);
		for (int i = 0; i < benchmark_size*benchmark_size; i++) {
			A[i] = A_value;
			B[i] = B_value;
		}
		for (int i = 0; i < benchmark_size*benchmark_size; i++) {
			C[i] = A[i] * B[i] - 1;
		}
		// Verification
		for (int i = 0; i < benchmark_size*benchmark_size; i++) {
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

void pull_down_gpios() {
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

void restart_all_ticks(void) {
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

void leverage_clock_source_rosc() {
	rosc_enable();
	xosc_init();
	clock_configure_undivided(clk_ref, CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC, 0, XOSC_HZ);
	restart_all_ticks();
	uint rosc_freq = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_ROSC_CLKSRC_PH);
	clock_configure_undivided(clk_ref, CLOCKS_CLK_REF_CTRL_SRC_VALUE_ROSC_CLKSRC_PH, 0, rosc_freq);
	clock_configure_undivided(clk_sys, CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX, CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_ROSC_CLKSRC, rosc_freq);
	clock_configure(clk_peri, 0, CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS, rosc_freq, rosc_freq);
	restart_all_ticks();
	pll_deinit(pll_sys);
	pll_deinit(pll_usb);
	xosc_disable();
}

uint8_t execute_benchmarks(uint bench_noop_size, uint bench_prime_size, uint bench_multi_size, uint bench_mat_size, uint bench_mat_float_size, uint bench_mat_double_size, uint nb_iteration_mat_mul) {
	if(watchdog_hw->scratch[3] == 0) {
		gpio_put(expe_pin, 1);
		benchmark_noop(bench_noop_size); // Uses one CPU core
		gpio_put(expe_pin, 0);
	}
	if(watchdog_hw->scratch[3] == 1) {
		gpio_put(expe_pin, 1);
		uint8_t result_prime = benchmark_prime(bench_prime_size); // Uses one CPU core
		gpio_put(expe_pin, 0);
	}
	sleep_us((int)(100000*TIME_RATE));
	if(watchdog_hw->scratch[3] == 2) {
		gpio_put(expe_pin, 1);
		uint8_t result_multicores = benchmark_prime_multicores(bench_multi_size); // Uses both cores
		gpio_put(expe_pin, 0);
	}
	sleep_us((int)(100000*TIME_RATE));
	if(watchdog_hw->scratch[3] == 3) {
		gpio_put(expe_pin, 1);
		uint8_t result_mat_mul = benchmark_mat_mul(bench_mat_size, nb_iteration_mat_mul); // Uses RAM
		gpio_put(expe_pin, 0);
	}
	sleep_us((int)(100000*TIME_RATE));
	if(watchdog_hw->scratch[3] == 4) {
		gpio_put(expe_pin, 1);
		uint8_t result_mat_mul_float = benchmark_mat_mul_float(bench_mat_float_size, nb_iteration_mat_mul); // Uses float co-processor
		gpio_put(expe_pin, 0);
	}
	if(watchdog_hw->scratch[3] == 5) {
		gpio_put(expe_pin, 1);
		uint8_t result_mat_mul_double = benchmark_mat_mul_double(bench_mat_double_size, nb_iteration_mat_mul); // Uses double co-processor
		gpio_put(expe_pin, 0);
	}
	watchdog_hw->scratch[3] = (watchdog_hw->scratch[3]+1)%6;
	return 1;
	//return result_prime|result_multicores<<1|result_mat_mul<<2|result_mat_mul_float<<3|result_mat_mul_double<<4;
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
