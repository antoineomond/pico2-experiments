#include "pico/stdlib.h"
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
#include <stdlib.h>
#include "experiments.h"

#define VALIDATION_RUN 0

// Benchmark sizes
#define BENCH_NOOP_SIZE 10
#define BENCH_PRIME_SIZE 5000
#define BENCH_MULTI_SIZE 5000
#define BENCH_MAT_SIZE 72
#define BENCH_MAT_FLOAT_SIZE 72
#define BENCH_MAT_DOUBLE_SIZE 36
#define NB_ITERATIONS_MAT_MUL 1000

#define NB_EXPES 14
#define LINE_SIZE 50

float TIME_RATE = 1;
extern const int expe_pin;

int main() {
	iteration_init();
	
	const uint vreg_output = VREG_VOLTAGE_0_75;
	vreg_set_voltage(vreg_output);
	clock_configure_undivided(clk_sys, CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX, CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_XOSC_CLKSRC, XOSC_HZ); // crash if changing pll while assigned on clk_sys 
	pll_deinit(pll_sys);
	pll_deinit(pll_usb);
	rosc_disable();
	sleep_ms(1000);
	
	uint8_t results = execute_benchmarks(BENCH_NOOP_SIZE, BENCH_PRIME_SIZE, BENCH_MULTI_SIZE, BENCH_MAT_SIZE, BENCH_MAT_FLOAT_SIZE, BENCH_MAT_DOUBLE_SIZE, NB_ITERATIONS_MAT_MUL);
	
	vreg_set_voltage(VREG_VOLTAGE_DEFAULT);
	
	while(true) {
		sleep_ms(600000);
	}
	
	return 0; // Should never reach here
}
