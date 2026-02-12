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

struct pll_params {
  uint vco_freq;
  uint div1;
  uint div2;
};
const struct pll_params expes[] = {
	// pll    760    5    5      30240
	// pll    1540   7    7      31347
	// pll    760    3    5      50400
	// pll    1520   5    6      50400
	// pll    990    1    7      70286
	// pll    1480   3    7      70285
	// pll    890    2    4     111000
	// pll    1550   2    7     110571
	// pll    780    1    6     130001
	// pll    1310   2    5     130800
	// pll    900    2    3     150000
	// pll    1500   5    2     150000
	{760,  5, 5}, //  30240 kHz
	{1540, 7, 7}, //  31347 kHz
	{760,  3, 5}, //  50400 kHz
	{1520, 5, 6}, //  50400 kHz
	{990,  1, 7}, //  70286 kHz
	{1480, 3, 7}, //  70285 kHz
	{890,  2, 4}, // 111000 kHz
	{1550, 2, 7}, // 110571 kHz
	{780,  1, 6}, // 130001 kHz
	{1310, 2, 5}, // 130800 kHz
	{900,  2, 3}, // 150000 kHz
	{1500, 5, 2}, // 150000 kHz
};

int main() {
	iteration_init();
	pll_deinit(pll_usb);
	rosc_disable();
	
	uint index_buff = 0;
	char** strings_buffer = malloc(sizeof(char*) * NB_EXPES);
	for (int i = 0; i < NB_EXPES; i++) {
		strings_buffer[i] = malloc(LINE_SIZE);
	}
	
	const uint expe_num = 11;
	clock_configure_undivided(clk_sys, CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX, CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_XOSC_CLKSRC, XOSC_HZ); // crash if changing pll while assigned on clk_sys 
	pll_init(pll_sys, PLL_SYS_REFDIV, expes[expe_num].vco_freq*MHZ, expes[expe_num].div1, expes[expe_num].div2);
	uint pll_freq = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_PLL_SYS_CLKSRC_PRIMARY)*1000;
	clock_configure_undivided(clk_sys, CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX, CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS, pll_freq); // crash if changing pll while assigned on clk_sys 
	//uint pll_freq = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_PLL_SYS_CLKSRC_PRIMARY)*1000;
	uint8_t results = execute_benchmarks(BENCH_NOOP_SIZE, BENCH_PRIME_SIZE, BENCH_MULTI_SIZE, BENCH_MAT_SIZE, BENCH_MAT_FLOAT_SIZE, BENCH_MAT_DOUBLE_SIZE, NB_ITERATIONS_MAT_MUL);
	
	//sprintf(strings_buffer[index_buff++], "%d,pll,%.2d,,%.2d,%x,%.4x,%.4x,,,,%b,%d,,,\n", watchdog_hw->scratch[1], expes[expe_num].vco_freq, expes[expe_num].div1, expes[expe_num].div2, results, pll_freq);
	//iteration_end(strings_buffer, index_buff);
	vreg_set_voltage(VREG_VOLTAGE_DEFAULT);
	
	while(true) {
		sleep_ms(600000);
	}
	
	return 0; // Should never reach here
}
