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

struct rosc_params {
  uint vreg;
  uint div;
  uint range;
  uint freqa;
  uint freqb;
};
const struct rosc_params expes[] = {
	// rosc   11       5   fa4  7777   577      30281
	// rosc    4       1   fa4   177     3      30276
	// rosc   11       4   fa7   177    57      50002
	// rosc    4       1   fa7    17    37      50005
	// rosc   10       2   fa4  1777  7777      70437
	// rosc    4       1   fa7  5777    57      70444
	// rosc   11       2   fa5  1777    17      90046
	// rosc    6       1   fa5  7777    17      90130
	// rosc   11       1   fa4   377     1     111038
	// rosc    7       1   fa5  7777    37     111094
	// rosc   11       1   fa4    17  5777     130215
	// rosc    8       1   fa5  5777    37     130380
	// rosc   11       1   fa5    17  3777     150107
	// rosc    9       1   fa5  5777    57     150100
	{VREG_VOLTAGE_1_10, 5, ROSC_CTRL_FREQ_RANGE_VALUE_LOW, 0x00007777, 0x00000577}, // 30281 kHz
	{VREG_VOLTAGE_0_75, 1, ROSC_CTRL_FREQ_RANGE_VALUE_LOW, 0x00000177, 0x00000003}, // 30276 kHz
	
	{VREG_VOLTAGE_1_10, 4, ROSC_CTRL_FREQ_RANGE_VALUE_HIGH, 0x00000177, 0x00000057}, // 50002 kHz
	{VREG_VOLTAGE_0_75, 1, ROSC_CTRL_FREQ_RANGE_VALUE_HIGH, 0x00000017, 0x00000037}, // 50005 kHz
	
	{VREG_VOLTAGE_1_05, 2, ROSC_CTRL_FREQ_RANGE_VALUE_LOW, 0x00001777, 0x00007777}, // 70437 kHz
	{VREG_VOLTAGE_0_75, 1, ROSC_CTRL_FREQ_RANGE_VALUE_HIGH, 0x00005777, 0x00000057}, // 70444 kHz
	
	{VREG_VOLTAGE_1_10, 2, ROSC_CTRL_FREQ_RANGE_VALUE_MEDIUM, 0x00001777, 0x00000017}, // 90046 kHz
	{VREG_VOLTAGE_0_85, 1, ROSC_CTRL_FREQ_RANGE_VALUE_MEDIUM, 0x00007777, 0x00000017}, // 90130 kHz
	
	{VREG_VOLTAGE_1_10, 1, ROSC_CTRL_FREQ_RANGE_VALUE_LOW, 0x00000377, 0x00000001}, // 111038 kHz
	{VREG_VOLTAGE_0_90, 1, ROSC_CTRL_FREQ_RANGE_VALUE_MEDIUM, 0x00007777, 0x00000037}, // 111094 kHz
	
	{VREG_VOLTAGE_1_10, 1, ROSC_CTRL_FREQ_RANGE_VALUE_LOW, 0x00000017, 0x00005777}, // 130215 kHz
	{VREG_VOLTAGE_0_95, 1, ROSC_CTRL_FREQ_RANGE_VALUE_MEDIUM, 0x00005777, 0x00000037}, // 130380 kHz
	
	{VREG_VOLTAGE_1_10, 1, ROSC_CTRL_FREQ_RANGE_VALUE_MEDIUM, 0x00000017, 0x00003777}, // 150107 kHz
	{VREG_VOLTAGE_1_00, 1, ROSC_CTRL_FREQ_RANGE_VALUE_MEDIUM, 0x00005777, 0x00000057}, // 150100 kHz
};

int main() {
	// Inspired from https://github.com/peterharperuk/pico-examples/commit/7dccd00d15ded4ddf961f44fdcd1f11a9d8c8be1
	iteration_init();
	sleep_ms(3000);
	leverage_clock_source_rosc();
	uint index_buff = 0;
	char** strings_buffer = malloc(sizeof(char*) * NB_EXPES);
	for (int i = 0; i < NB_EXPES; i++) {
		strings_buffer[i] = malloc(LINE_SIZE);
	}
	
	uint expe_num = 13;
	
	// Setup parameters
	vreg_set_voltage(expes[expe_num].vreg);
	rosc_set_div(expes[expe_num].div);
	rosc_set_range(expes[expe_num].range);
	rosc_write(&rosc_hw->freqa, (ROSC_FREQA_PASSWD_VALUE_PASS << ROSC_FREQA_PASSWD_LSB) | expes[expe_num].freqa);
	rosc_write(&rosc_hw->freqb, (ROSC_FREQA_PASSWD_VALUE_PASS << ROSC_FREQA_PASSWD_LSB) | expes[expe_num].freqb);

	// Count rosc freq
	// Make clk_ref have a stable clock to count frequency  
	xosc_init();
	clock_configure_undivided(clk_ref, CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC, 0, XOSC_HZ);
	restart_all_ticks();
	uint rosc_freq = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_ROSC_CLKSRC)*1000;

	// Make rosc as clk_ref, then disable XOSC as it is no longer needed
	clock_configure_undivided(clk_ref, CLOCKS_CLK_REF_CTRL_SRC_VALUE_ROSC_CLKSRC_PH, 0, rosc_freq);
	restart_all_ticks();
	xosc_disable();
	
	clock_set_reported_hz(clk_ref, rosc_freq);
	clock_set_reported_hz(clk_sys, rosc_freq);
	
	sleep_ms(3000);
	
	uint results = 1;
	gpio_put(expe_pin, 1);
	benchmark_noop(BENCH_NOOP_SIZE);
	gpio_put(expe_pin, 0);
	sleep_ms(100);
	gpio_put(expe_pin, 1);
	uint8_t result_prime = benchmark_prime(BENCH_PRIME_SIZE); // Uses one CPU core
	gpio_put(expe_pin, 0);
	sleep_ms(100);
	gpio_put(expe_pin, 1);
	uint8_t result_multicores = benchmark_prime_multicores(BENCH_MULTI_SIZE); // Uses both cores
	gpio_put(expe_pin, 0);
	sleep_ms(100);
	gpio_put(expe_pin, 1);
	uint8_t result_mat_mul = benchmark_mat_mul(BENCH_MAT_SIZE, NB_ITERATIONS_MAT_MUL); // Uses RAM
	gpio_put(expe_pin, 0);
	sleep_ms(100);
	gpio_put(expe_pin, 1);
	uint8_t result_mat_mul_float = benchmark_mat_mul_float(BENCH_MAT_FLOAT_SIZE, NB_ITERATIONS_MAT_MUL); // Uses float co-processor
	gpio_put(expe_pin, 0);
	sleep_ms(100);
	gpio_put(expe_pin, 1);
	uint8_t result_mat_mul_double = benchmark_mat_mul_double(BENCH_MAT_DOUBLE_SIZE, NB_ITERATIONS_MAT_MUL); // Uses double co-processor
	gpio_put(expe_pin, 0);
	sleep_ms(100);
	
	sprintf(strings_buffer[index_buff++], "%d,rosc,%.2d,,%.2d,%x,%.4x,%.4x,,,,%b,%d,,,\n", watchdog_hw->scratch[1], expes[expe_num].vreg, expes[expe_num].div, expes[expe_num].range, expes[expe_num].freqa, expes[expe_num].freqb, results, rosc_freq);
	//iteration_end(strings_buffer, index_buff);
	vreg_set_voltage(VREG_VOLTAGE_DEFAULT);
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
	while(true) {
		sleep_ms(60000);
	}
	
	return 0; // Should never reach here
}
