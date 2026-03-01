#include "experiments.h"

const struct params expes_same_freqs[] = {
	{PLL_SYS, 760*MHZ,  5, 5, 0, 0, 0, 0, 0, VREG_DEFAULT},
	{PLL_SYS, 1540*MHZ, 7, 7, 0, 0, 0, 0, 0, VREG_DEFAULT},
	{PLL_SYS, 760*MHZ,  5, 5, 0, 0, 0, 0, 0, VREG_MIN_PLL},
	{PLL_SYS, 1540*MHZ, 7, 7, 0, 0, 0, 0, 0, VREG_MIN_PLL},
};
