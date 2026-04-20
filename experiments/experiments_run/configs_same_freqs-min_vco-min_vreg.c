#include "target_configuration.h"

const struct config expes_same_freqs_min_vco_min_vreg[] = {
	// 30 MHz, min VREG
	{PLL_SYS, 760*MHZ, 5, 5, 0, 0, 0, 0, 0, VREG_VOLTAGE_0_90, true},
	{ROSC, 0, 0, 0, 2, 0xfa7, 0x0003, 0x0017, 0, VREG_VOLTAGE_0_80, true},
	// 50MHz
	{PLL_SYS, 760*MHZ, 3, 5, 0, 0, 0, 0, 0, VREG_VOLTAGE_0_90, true},
	{ROSC, 0, 0, 0, 1, 0xfa5, 0x0017, 0x0005, 0, VREG_VOLTAGE_0_80, true},
	// 70MHz
	//{PLL_SYS, 1480*MHZ, 3, 7, 0, 0, 0, 0, 0, VREG_MIN_PLL, true},
	// TODO: find rosc value with default vreg output
	// 110MHz
	{PLL_SYS, 770*MHZ, 1, 7, 0, 0, 0, 0, 0, VREG_VOLTAGE_0_90, true},
	{ROSC, 0, 0, 0, 1, 0xfa5, 0x7777, 0x0037, 0, VREG_VOLTAGE_0_90, true},
	// 130 MHz
	{PLL_SYS, 780*MHZ, 2, 3, 0, 0, 0, 0, 0, VREG_VOLTAGE_0_90, true},
	{ROSC, 0, 0, 0, 1, 0xfa5, 0x5777, 0x0037, 0, VREG_VOLTAGE_0_95, true},
	// 150 MHz
	{PLL_SYS, 900*MHZ, 2, 3, 0, 0, 0, 0, 0, VREG_VOLTAGE_0_90, true},
	{ROSC, 0, 0, 0, 1, 0xfa5, 0x5777, 0x0057, 0, VREG_VOLTAGE_1_00, true},
};
const uint size_expes_same_freqs_min_vco_min_vreg = sizeof(expes_same_freqs_min_vco_min_vreg)/sizeof(expes_same_freqs_min_vco_min_vreg[0]);
