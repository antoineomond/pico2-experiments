#include "pico/stdlib.h"

uint8_t benchmark_prime(uint benchmark_size);
uint8_t benchmark_prime_multicores(uint benchmark_size);
uint8_t benchmark_mat_mul(unsigned int benchmark_size);
uint8_t benchmark_mat_mul_float(unsigned int benchmark_size);
uint8_t benchmark_mat_mul_double(unsigned int benchmark_size);

// Clock source leverages
void leverage_clock_source_lposc();
void restart_all_ticks(void);

// Additional leverages (turn off unused stuff)
void pull_down_gpios(uint expe_pin);
void turn_off_clocks();
static void disable_usb();

// Sleep mode
void processor_deep_sleep(void);
