#include "pico/stdlib.h"

void iteration_init();
void iteration_end(const char* expe_params, uint clock_freq);

uint8_t benchmark_prime(uint benchmark_size);
uint8_t benchmark_prime_multicores(uint benchmark_size);
uint8_t benchmark_mat_mul(uint benchmark_size, uint nb_iteration_mat_mul);
uint8_t benchmark_mat_mul_float(uint benchmark_size, uint nb_iteration_mat_mul);
uint8_t benchmark_mat_mul_double(uint benchmark_size, uint nb_iteration_mat_mul);
void execute_benchmarks(uint bench_noop_size, uint bench_prime_size, uint bench_multi_size, uint bench_mat_size, uint bench_mat_foat_size, uint bench_mat_double_size, uint nb_iteration_mat_mul);

// Clock source leverages
void leverage_clock_source_lposc();
void restart_all_ticks(void);

// Additional leverages (turn off unused stuff)
void pull_down_gpios();
void turn_off_clocks();
static void disable_usb();

// Sleep mode
void processor_deep_sleep(void);
