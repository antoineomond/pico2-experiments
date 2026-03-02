#include <stdio.h>
#include "experiments.h"

// Select experiments and phase to run
#define EXPES_PARAMS expes_baseline
#define NB_EXPES size_expes_baseline
#define PHASE 1
#define LINE_SIZE 50

extern const struct params expes_baseline[];
extern const uint size_expes_baseline;
extern const struct params expes_minimums[];
extern const uint size_expes_minimums;
extern const struct params expes_same_freqs[];
extern const uint size_expes_same_freqs;

int main() {
	iteration_init(PHASE);
	const struct params *expes = expes_baseline;
	uint nb_expes = NB_EXPES;
	uint expe_num = watchdog_hw->scratch[1]%nb_expes;
	char buffer[LINE_SIZE];
	
	// Set default voltage
	// Voltage much be set before clock sources and frequency 
	vreg_set_voltage(expes_baseline[expe_num].vreg_output);

	// Set clock source with default frequencies
	uint clock_freq;
	if(expes[expe_num].clock_source == PLL_SYS) {
		leverage_clock_source_pll(expes[expe_num].pll_vco_freq, expes[expe_num].pll_div1, expes[expe_num].pll_div2);
		clock_freq = expes[expe_num].pll_vco_freq / (expes[expe_num].pll_div1*expes[expe_num].pll_div2);
	}
	if(expes[expe_num].clock_source == XOSC) {
		leverage_clock_source_xosc();
		clock_freq = 12*MHZ;
	}
	if(expes[expe_num].clock_source == ROSC) {
		leverage_clock_source_rosc(expes[expe_num].rosc_div, expes[expe_num].rosc_range, expes[expe_num].rosc_drive_freqa, expes[expe_num].rosc_drive_freqb, &clock_freq);
	}
	if(expes[expe_num].clock_source == LPOSC) {
		leverage_clock_source_lposc(expes[expe_num].lposc_trim, &clock_freq);
	}
	bool clock_source_lposc = expes[expe_num].clock_source == LPOSC ? true : false;
	
	uint8_t results = execute_benchmarks(clock_source_lposc);
	sprintf(buffer, "%d,%d,%d,%b\n",expe_num,expes[expe_num].clock_source,clock_freq,results);
	
	iteration_end(PHASE, buffer);
	return 0; // Should never reach here
}
