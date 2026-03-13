#include <stdio.h>
#include "experiments.h"

// Select experiments and phase to run
#define EXPES_PARAMS expes_minimums
#define NB_EXPES size_expes_minimums
#define PHASE 1

#define LINE_SIZE 50
extern const struct params expes_baseline[];
extern const uint size_expes_baseline;
extern const struct params expes_minimums[];
extern const uint size_expes_minimums;
extern const struct params expes_same_freqs[];
extern const uint size_expes_same_freqs;
extern float TIME_RATE;

int main() {
	iteration_init(PHASE);
	const struct params *expes = EXPES_PARAMS;
	uint nb_expes = NB_EXPES;
	uint expe_num = watchdog_hw->scratch[1]%nb_expes;
	char buffer[LINE_SIZE];
	
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
		leverage_clock_source_rosc(expes[expe_num].rosc_div, expes[expe_num].rosc_range, expes[expe_num].rosc_drive_freqa, expes[expe_num].rosc_drive_freqb, &clock_freq, expes[expe_num].set_as_ref);
	}
	if(expes[expe_num].clock_source == LPOSC) {
		leverage_clock_source_lposc(expes[expe_num].lposc_trim, &clock_freq, expes[expe_num].set_as_ref);
	}
	bool clock_source_lposc = expes[expe_num].clock_source == LPOSC ? true : false;
	
	// Set default voltage
	// Voltage much be set after clock sources and frequency to reach values below 0.9V
	vreg_set_voltage(expes[expe_num].vreg_output);
	
	uint clk_src_freq;
	if(expes[expe_num].clock_source == PLL_SYS) clk_src_freq = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_PLL_SYS_CLKSRC_PRIMARY) * KHZ;
	if(expes[expe_num].clock_source == XOSC) clk_src_freq = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_XOSC_CLKSRC) * KHZ;
	if(expes[expe_num].clock_source == ROSC) clk_src_freq = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_ROSC_CLKSRC_PH) * KHZ;
	if(expes[expe_num].clock_source == LPOSC) clk_src_freq = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_LPOSC_CLKSRC) * KHZ;
	
	if(expes[expe_num].set_as_ref && (expes[expe_num].clock_source == ROSC || expes[expe_num].clock_source == LPOSC)) {
		clock_set_reported_hz(clk_ref, clk_src_freq);
		restart_all_ticks();
		TIME_RATE = ((float)((float)clk_src_freq/(float)MHZ))/((float)(clk_src_freq/MHZ)); // clk_ref takes clock_freq/MHz as reference to compute time, trimming all remaining KHz. This leads to incorrect time tracking  
	}
	clock_set_reported_hz(clk_sys, clk_src_freq);
	
	uint8_t results = execute_benchmarks(clock_source_lposc);
	sprintf(buffer, "%d,%d,%d,%b\n",expe_num,expes[expe_num].clock_source,clock_freq,results);
	
	iteration_end(PHASE, buffer);
	return 0; // Should never reach here
}
