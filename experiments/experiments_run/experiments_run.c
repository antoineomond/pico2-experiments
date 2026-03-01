#include "experiments.h"

// Select experiments to run and phase
#define EXPES_PARAMS expes_diff_freqs
#define PHASE 0

extern const struct params expes_diff_freqs[];
extern const struct params expes_same_freqs[];

int main() {
	iteration_init();
	const struct params *expes = EXPES_PARAMS;
	uint nb_expes = sizeof(expes)/sizeof(expes[0]);
	uint expe_num = watchdog_hw->scratch[1]%nb_expes;
	
	// Set default voltage
	// Voltage much be set before clock sources and frequency 
	vreg_set_voltage(expes[expe_num].vreg_output);

	// Set clock source with default frequencies
	if(expes[expe_num].clock_source == PLL_SYS) leverage_clock_source_pll(expes[expe_num].pll_vco_freq, expes[expe_num].pll_div1, expes[expe_num].pll_div2);
	if(expes[expe_num].clock_source == XOSC) leverage_clock_source_xosc();
	if(expes[expe_num].clock_source == ROSC) leverage_clock_source_rosc(expes[expe_num].rosc_div, expes[expe_num].rosc_drive_strength, expes[expe_num].rosc_range_freqa, expes[expe_num].rosc_range_freqb);
	if(expes[expe_num].clock_source == LPOSC) leverage_clock_source_lposc(expes[expe_num].lposc_trim);
	bool clock_source_lposc = expes[expe_num].clock_source == LPOSC ? true : false;
	
	uint8_t results = execute_benchmarks(clock_source_lposc);
	log_experiment_result("%d,pll,%.2d,,%.2d,%x,%.4x,%.4x,,,,%b,%d,,,\n", expe_num, PLL_SYS_VCO_FREQ_HZ, PLL_SYS_POSTDIV1, PLL_SYS_POSTDIV2, results, 150*MHZ);
	
	iteration_end();
	return 0; // Should never reach here
}
