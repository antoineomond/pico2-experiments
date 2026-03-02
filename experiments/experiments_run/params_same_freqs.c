#include "experiments.h"

const struct params expes_same_freqs[] = {
	// 30 MHz
	{PLL_SYS, 760*MHZ,  5, 5, 0, 0, 0, 0, 0, VREG_DEFAULT},
	{PLL_SYS, 1540*MHZ, 7, 7, 0, 0, 0, 0, 0, VREG_DEFAULT},
	{PLL_SYS, 760*MHZ,  5, 5, 0, 0, 0, 0, 0, VREG_MIN_PLL},
	{PLL_SYS, 1540*MHZ, 7, 7, 0, 0, 0, 0, 0, VREG_MIN_PLL},
};
const uint size_expes_same_freqs = sizeof(expes_same_freqs)/sizeof(expes_same_freqs[0]);
