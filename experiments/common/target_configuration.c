#include "experiments.h"

struct result {
	params p;
	uint power_median_mw;
};

const struct result results[] = {
	// Baseline
	{{PLL_SYS, PLL_DEFAULT_VCO_FREQ_HZ, PLL_DEFAULT_POSTDIV1, PLL_DEFAULT_POSTDIV2, 0, 0, 0, 0, 0, VREG_DEFAULT, true}, 1.8},
	{{XOSC, 0, 0, 0, 0, 0, 0, 0, 0, VREG_DEFAULT, true}, 1},
	{{ROSC, 0, 0, 0, ROSC_DEFAULT_DIVIDER, ROSC_DEFAULT_RANGE, ROSC_DEFAULT_DRIVE_STRENGTH, ROSC_DEFAULT_DRIVE_STRENGTH, 0, VREG_DEFAULT, true}, 2}
};
const uint size_results = sizeof(results)/sizeof(results[0]);

void switch_configuration(uint target_power_median_mw) {
	// Search closest configuration that make the rpi use a power median lower than target_power_median_mw 
	result res = NULL;
	for (uint i = 0; i < size_results; i++) {
		if(results[i].power_median_mw <= target_power_median_mw) {
			if(res == NULL) {
				res = results[i];
			}
			else if (res.power_median_mw <= results[i].power_median_mw) {
				res = results[i];
			}
		}
	}
	
	// Set the voltage, clock source and frequency (measure the frequency for rosc and lposc)
	vreg_set_voltage(res.p.vreg_output);
	uint clock_freq;
	if(res.p.clock_source == PLL_SYS) {
		leverage_clock_source_pll(res.p.pll_vco_freq, res.p.pll_div1, res.p.pll_div2);
		clock_freq = res.p.pll_vco_freq / (res.p.pll_div1*res.p.pll_div2);
	}
	if(res.p.clock_source == XOSC) {
		leverage_clock_source_xosc();
		clock_freq = 12*MHZ;
	}
	if(res.p.clock_source == ROSC) {
		leverage_clock_source_rosc(res.p.rosc_div, res.p.rosc_range, res.p.rosc_drive_freqa, res.p.rosc_drive_freqb, &clock_freq, res.p.set_as_ref);
	}
	if(res.p.clock_source == LPOSC) {
		leverage_clock_source_lposc(res.p.lposc_trim, &clock_freq, res.p.set_as_ref);
	}
}
