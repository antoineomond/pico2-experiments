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

#define NB_EXPES 4
#define PHASE 0

int main() {
	iteration_init();
	uint expe_num = watchdog_hw->scratch[1];
	
	// Set default voltage
	// Voltage much be set before clock sources and frequency 
	vreg_set_voltage(VREG_VOLTAGE_DEFAULT);

	// Set clock source with default frequencies
	if(expe_num == 0) leverage_clock_source_pll(PLL_SYS_VCO_FREQ_HZ, PLL_SYS_POSTDIV1, PLL_SYS_POSTDIV2);
	if(expe_num == 1) leverage_clock_source_xosc();
	if(expe_num == 2) leverage_clock_source_rosc(ROSC_DEFAULT_DIVIDER, ROSC_DEFAULT_DRIVE_STRENGTH, ROSC_DEFAULT_RANGE, ROSC_DEFAULT_RANGE);
	if(expe_num == 3) leverage_clock_source_lposc(LPOSC_DEFAULT_TRIM);
	bool clock_source_lposc = expe_num == 3 ? true : false;
	
	uint8_t results = execute_benchmarks(clock_source_lposc);
	log_experiment_result("%d,pll,%.2d,,%.2d,%x,%.4x,%.4x,,,,%b,%d,,,\n", expe_num, PLL_SYS_VCO_FREQ_HZ, PLL_SYS_POSTDIV1, PLL_SYS_POSTDIV2, results, 150*MHZ);
	
	iteration_end();
	return 0; // Should never reach here
}
