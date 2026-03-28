#include "pico/stdlib.h"
#include "hardware/pll.h"
#include "hardware/vreg.h"
#include "hardware/powman.h"
#include "hardware/pll.h"
#include "hardware/xosc.h"
#include "hardware/watchdog.h"
#include "pico/sleep.h"
#include "hardware/clocks.h"

#define PLL_MIN_VCO_FREQ_HZ 760*MHZ 
#define PLL_MAX_POSTDIV 7 
#define PLL_DEFAULT_VCO_FREQ_HZ PLL_SYS_VCO_FREQ_HZ 
#define PLL_DEFAULT_POSTDIV1 PLL_SYS_POSTDIV1
#define PLL_DEFAULT_POSTDIV2 PLL_SYS_POSTDIV2

#define ROSC_MAX_DIVIDER 30
#define ROSC_MIN_RANGE ROSC_CTRL_FREQ_RANGE_VALUE_LOW
#define ROSC_MIN_DRIVE_STRENGTH 0x0000
#define ROSC_MAX_DRIVE_STRENGTH 0x7777
#define ROSC_DEFAULT_DIVIDER 8
#define ROSC_DEFAULT_RANGE ROSC_MIN_RANGE
#define ROSC_DEFAULT_DRIVE_STRENGTH ROSC_MIN_DRIVE_STRENGTH

#define LPOSC_MIN_TRIM 0x000
#define LPOSC_MAX_TRIM 0x3f0
#define LPOSC_DEFAULT_TRIM LPOSC_MIN_TRIM

#define VREG_DEFAULT VREG_VOLTAGE_DEFAULT
#define VREG_MIN_PLL VREG_VOLTAGE_0_90
#define VREG_MIN_XOSC_ROSC_LPOSC VREG_VOLTAGE_0_75

enum CLOCK_SOURCE {
	PLL_SYS, XOSC, ROSC, LPOSC
};

struct params {
	// Clock source
	uint clock_source;
	
	// Clock frequency
	// PLL
  uint pll_vco_freq;
  uint pll_div1;
  uint pll_div2;
	
	// ROSC
  uint rosc_div;
  uint rosc_range;
  uint rosc_drive_freqa;
  uint rosc_drive_freqb;
	
	// LPOSC
	uint lposc_trim;
	
	// VREG output
	uint vreg_output;
	
	// Set clock source as reference clock
	bool set_as_ref;
};

void iteration_init(uint phase, uint vreg_expe);
void iteration_end(uint phase, char* buffer);

uint8_t benchmark_noop(uint benchmark_size);
uint8_t benchmark_prime(uint benchmark_size);
uint8_t benchmark_prime_multicores(uint benchmark_size);
uint8_t benchmark_mat_mul(uint benchmark_size, uint nb_iteration_mat_mul);
uint8_t benchmark_mat_mul_float(uint benchmark_size, uint nb_iteration_mat_mul);
uint8_t benchmark_mat_mul_double(uint benchmark_size, uint nb_iteration_mat_mul);
uint8_t execute_benchmarks(bool clock_source_lposc);

// Others
void led_blink(uint count);
