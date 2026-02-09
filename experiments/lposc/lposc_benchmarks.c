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

// Experiment parameters
#define TARGET_VOLTAGE VREG_VOLTAGE_0_75
#define TARGET_LPOSC_TRIM 0x3f0

#define VALIDATION_RUN 1

// Benchmark sizes
#define BENCH_NOOP_SIZE 20
#define BENCH_PRIME_SIZE 200
#define BENCH_MULTI_SIZE 200
#define BENCH_MAT_SIZE 72
#define BENCH_MAT_FLOAT_SIZE 72
#define BENCH_MAT_DOUBLE_SIZE 36
#define NB_ITERATIONS_MAT_MUL 1

float TIME_RATE = 1;

int main() {
	// Inspired from https://github.com/peterharperuk/pico-examples/commit/7dccd00d15ded4ddf961f44fdcd1f11a9d8c8be1
	iteration_init();
	leverage_clock_source_lposc();
	
	// Setup voltage and trimming
	vreg_set_voltage(TARGET_VOLTAGE);
	powman_clear_bits(&powman_hw->lposc, POWMAN_LPOSC_TRIM_BITS);
	powman_set_bits(&powman_hw->lposc, POWMAN_LPOSC_TRIM_BITS & TARGET_LPOSC_TRIM);

	// Count lposc freq
	// Make clk_ref have a stable clock to count frequency  
	xosc_init();
	clock_configure_undivided(clk_ref, CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC, 0, XOSC_HZ);
	restart_all_ticks();
	uint lposc_freq = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_LPOSC_CLKSRC)*KHZ;
	TIME_RATE = ((float)lposc_freq)/((float)1*MHZ); // Adjust TIME_RATE to account the very slow freq of lposc

	// Make lposc as clk_ref, then disable XOSC as it is no longer needed
	clock_configure_undivided(clk_ref, CLOCKS_CLK_REF_CTRL_SRC_VALUE_LPOSC_CLKSRC, 0, lposc_freq);
	restart_all_ticks();
	xosc_disable();
	
	clock_set_reported_hz(clk_ref, lposc_freq);
	clock_set_reported_hz(clk_sys, lposc_freq);
	
	execute_benchmarks(BENCH_NOOP_SIZE, BENCH_PRIME_SIZE, BENCH_MULTI_SIZE, BENCH_MAT_SIZE, BENCH_MAT_FLOAT_SIZE, BENCH_MAT_DOUBLE_SIZE, NB_ITERATIONS_MAT_MUL);
	
	// clock_source,vreg,lposc_trim,rosc_div,rosc_range,rosc_freqa,rosc_freqb,pll_vco,pll_div
	char* buf[1];
	sprintf(buf[0], "rosc,%.2d,%.3x,,,,,,", TARGET_VOLTAGE, TARGET_LPOSC_TRIM);
	
	// TODO rework this part
	iteration_end(buf, lposc_freq);
	
	return 0; // Should never reach here
}
