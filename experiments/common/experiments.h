#include "pico/stdlib.h"

#define ROSC_DEFAULT_DIVIDER 8
#define ROSC_DEFAULT_DRIVE_STRENGTH 0
#define ROSC_DEFAULT_RANGE ROSC_CTRL_FREQ_RANGE_VALUE_LOW
#define LPOSC_DEFAULT_TRIM 0

void iteration_init();
void iteration_end();

uint8_t benchmark_noop(uint benchmark_size);
uint8_t benchmark_prime(uint benchmark_size);
uint8_t benchmark_prime_multicores(uint benchmark_size);
uint8_t benchmark_mat_mul(uint benchmark_size, uint nb_iteration_mat_mul);
uint8_t benchmark_mat_mul_float(uint benchmark_size, uint nb_iteration_mat_mul);
uint8_t benchmark_mat_mul_double(uint benchmark_size, uint nb_iteration_mat_mul);
uint8_t execute_benchmarks(bool clock_source_lposc);

// Clock source leverages
void leverage_clock_source_lposc(uint trim);
void leverage_clock_source_rosc(uint div, uint range, uint freqa, uint freqb);
void leverage_clock_source_xosc();
void leverage_clock_source_pll(uint vco_freq, uint div1, uint div2);
void restart_all_ticks(void);

// Additional leverages (turn off unused stuff)
void pull_down_gpios();
void turn_off_clocks();
static void disable_usb();

// Sleep mode
void processor_deep_sleep(void);

// Others
void led_blink(uint count);
void log_experiment_result(const char * format, ...);
