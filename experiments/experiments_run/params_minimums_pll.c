#include "experiments.h"

const struct params expes_minimums_pll[] = {
	// Min frequency, default voltage
	{PLL_SYS, PLL_MIN_VCO_FREQ_HZ, PLL_MAX_POSTDIV, PLL_MAX_POSTDIV, 0, 0, 0, 0, 0, VREG_DEFAULT, true},
	
	// Default frequency, min voltage
	{PLL_SYS, PLL_DEFAULT_VCO_FREQ_HZ, PLL_DEFAULT_POSTDIV1, PLL_DEFAULT_POSTDIV2, 0, 0, 0, 0, 0, VREG_MIN_PLL, true},
	
	// Min frequency, min voltage
	{PLL_SYS, PLL_MIN_VCO_FREQ_HZ, PLL_MAX_POSTDIV, PLL_MAX_POSTDIV, 0, 0, 0, 0, 0, VREG_MIN_PLL, true},
};
const uint size_expes_minimums_pll = sizeof(expes_minimums_pll)/sizeof(expes_minimums_pll[0]);
