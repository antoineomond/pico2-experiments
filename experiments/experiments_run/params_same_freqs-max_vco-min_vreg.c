#include "target_configuration.h"

const struct params expes_same_freqs_max_vco_min_vreg[] = {
	// 30 MHz, min VREG
	{PLL_SYS, 1540*MHZ, 7, 7, 0, 0, 0, 0, 0, VREG_MIN_PLL, true},
	{ROSC, 0, 0, 0, 2, 0xfa7, 0x0005, 0x0007, 0, VREG_VOLTAGE_0_80, true},
	// 50MHz
	{PLL_SYS, 1520*MHZ, 5, 6, 0, 0, 0, 0, 0, VREG_MIN_PLL, true},
	{ROSC, 0, 0, 0, 1, 0xfa5, 0x0017, 0x0005, 0, VREG_VOLTAGE_0_80, true},
	// 70MHz
	//{PLL_SYS, 1480*MHZ, 3, 7, 0, 0, 0, 0, 0, VREG_MIN_PLL, true},
	// TODO: find rosc value with default vreg output
	// 110MHz
	{PLL_SYS, 1550*MHZ, 2, 7, 0, 0, 0, 0, 0, VREG_MIN_PLL, true},
	{ROSC, 0, 0, 0, 1, 0xfa5, 0x7777, 0x0037, 0, VREG_VOLTAGE_0_90, true},
	// 130 MHz
	{PLL_SYS, 1310*MHZ, 2, 5, 0, 0, 0, 0, 0, VREG_MIN_PLL, true},
	{ROSC, 0, 0, 0, 1, 0xfa5, 0x5777, 0x0037, 0, VREG_VOLTAGE_0_95, true},
	// 150
	{PLL_SYS, 1500*MHZ, 5, 2, 0, 0, 0, 0, 0, VREG_MIN_PLL, true},
	{ROSC, 0, 0, 0, 1, 0xfa5, 0x5777, 0x0057, 0, VREG_VOLTAGE_1_00, true},
};
const uint size_expes_same_freqs_max_vco_min_vreg = sizeof(expes_same_freqs_max_vco_min_vreg)/sizeof(expes_same_freqs_max_vco_min_vreg[0]);
