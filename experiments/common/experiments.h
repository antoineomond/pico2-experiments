#include "pico/stdlib.h"
#include "hardware/pll.h"
#include "hardware/vreg.h"
#include "hardware/powman.h"
#include "hardware/pll.h"
#include "hardware/xosc.h"
#include "hardware/watchdog.h"
#include "pico/sleep.h"
#include "hardware/clocks.h"

void iteration_init(uint phase, uint vreg_expe);
void iteration_end(uint phase, char* buffer);

uint8_t benchmark_noop(uint benchmark_size);
uint8_t benchmark_prime(uint benchmark_size);
uint8_t benchmark_prime_multicores(uint benchmark_size);
uint8_t benchmark_mat_mul(uint benchmark_size, uint nb_iteration_mat_mul);
uint8_t benchmark_mat_mul_float(uint benchmark_size, uint nb_iteration_mat_mul);
uint8_t benchmark_mat_mul_double(uint benchmark_size, uint nb_iteration_mat_mul);
uint8_t execute_benchmarks(bool clock_source_lposc, uint8_t benchmarks_to_run);

// Others
void led_blink(uint count);
void pull_down_gpios();
void turn_off_clocks();
static void disable_usb();

// Sleep mode
void processor_deep_sleep(void);

void print_configurations_csv(char* buffer, void* configurations, uint nb_expes, uint8_t benchmarks_to_run, uint clock_freq);
