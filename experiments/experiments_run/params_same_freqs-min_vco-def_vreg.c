#include "experiments.h"

const struct params expes_same_freqs_min_vco_def_vreg[] = {
	// 30 MHz, default VREG
	{PLL_SYS, 760*MHZ, 5, 5, 0, 0, 0, 0, 0, VREG_DEFAULT, true},
	{ROSC, 0, 0, 0, 5, 0xfa4, 0x7777, 0x0577, 0, VREG_DEFAULT, true},
	// 50MHz
	{PLL_SYS, 760*MHZ, 3, 5, 0, 0, 0, 0, 0, VREG_DEFAULT, true},
	{ROSC, 0, 0, 0, 4, 0xfa7, 0x0177, 0x0057, 0, VREG_DEFAULT, true},
	// 70MHz
	//{PLL_SYS, 1480*MHZ, 3, 7, 0, 0, 0, 0, 0, VREG_DEFAULT, true},
	// TODO: find rosc value with default vreg output
	// 110MHz
	{PLL_SYS, 770*MHZ, 1, 7, 0, 0, 0, 0, 0, VREG_DEFAULT, true},
	{ROSC, 0, 0, 0, 1, 0xfa4, 0x0377, 0x0001, 0, VREG_DEFAULT, true},
	// 130 MHz
	{PLL_SYS, 780*MHZ, 2, 3, 0, 0, 0, 0, 0, VREG_DEFAULT, true},
	{ROSC, 0, 0, 0, 1, 0xfa4, 0x0017, 0x5777, 0, VREG_DEFAULT, true},
	// 150 MHz
	{PLL_SYS, 900*MHZ, 2, 3, 0, 0, 0, 0, 0, VREG_DEFAULT, true},
	{ROSC, 0, 0, 0, 1, 0xfa5, 0x0017, 0x3777, 0, VREG_DEFAULT, true},
};
const uint size_expes_same_freqs_min_vco_def_vreg = sizeof(expes_same_freqs_min_vco_def_vreg)/sizeof(expes_same_freqs_min_vco_def_vreg[0]);
