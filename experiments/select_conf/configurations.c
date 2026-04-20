#include "target_configuration.h"

const struct config configs[] = {
	// Baseline
	{PLL_SYS, PLL_DEFAULT_VCO_FREQ_HZ, PLL_DEFAULT_POSTDIV1, PLL_DEFAULT_POSTDIV2, 0, 0, 0, 0, 0, VREG_DEFAULT, true},															 // 73.99 mW
	{XOSC, 0, 0, 0, 0, 0, 0, 0, 0, VREG_DEFAULT, true},																																															 // 14.85 mW
	{ROSC, 0, 0, 0, ROSC_DEFAULT_DIVIDER, ROSC_DEFAULT_RANGE, ROSC_DEFAULT_DRIVE_STRENGTH, ROSC_DEFAULT_DRIVE_STRENGTH, 0, VREG_DEFAULT, true},			 // 13.14 mW
	
	// pll Min frequency, default voltage
	{PLL_SYS, PLL_MIN_VCO_FREQ_HZ, PLL_MAX_POSTDIV, PLL_MAX_POSTDIV, 0, 0, 0, 0, 0, VREG_DEFAULT, true},																						 // 18.10 mW
	// pll Default frequency, min voltage
	{PLL_SYS, PLL_DEFAULT_VCO_FREQ_HZ, PLL_DEFAULT_POSTDIV1, PLL_DEFAULT_POSTDIV2, 0, 0, 0, 0, 0, VREG_MIN_PLL, true},															 // 54.18 mW
	// pll Min frequency, min voltage
	{PLL_SYS, PLL_MIN_VCO_FREQ_HZ, PLL_MAX_POSTDIV, PLL_MAX_POSTDIV, 0, 0, 0, 0, 0, VREG_MIN_PLL, true},																						 // 22.62 mW
	
	// rosc Min frequency, default voltage
	{ROSC, 0, 0, 0, ROSC_MAX_DIVIDER, ROSC_MIN_RANGE, ROSC_MIN_DRIVE_STRENGTH, ROSC_MIN_DRIVE_STRENGTH, 0, VREG_DEFAULT, true},											 // 9.82 mW
	// rosc Default frequency, min voltage
	{ROSC, 0, 0, 0, ROSC_DEFAULT_DIVIDER, ROSC_DEFAULT_RANGE, ROSC_DEFAULT_DRIVE_STRENGTH, ROSC_DEFAULT_DRIVE_STRENGTH, 0, VREG_VOLTAGE_0_80, true}, // 5.94 mW
	// rosc Min frequency, min voltage
	{ROSC, 0, 0, 0, ROSC_MAX_DIVIDER, ROSC_MIN_RANGE, ROSC_MIN_DRIVE_STRENGTH, ROSC_MIN_DRIVE_STRENGTH, 0, VREG_VOLTAGE_0_80, true},					       // 4.24 mW
	
	// lposc Default trim
	{LPOSC, 0, 0, 0, 0, 0, 0, 0, 0x20, VREG_VOLTAGE_1_10, true}, // 5.48 mW
	{LPOSC, 0, 0, 0, 0, 0, 0, 0, 0x20, VREG_VOLTAGE_1_00, true}, // 4.53 mW
	{LPOSC, 0, 0, 0, 0, 0, 0, 0, 0x20, VREG_VOLTAGE_0_90, true}, // 3.89 mW
	{LPOSC, 0, 0, 0, 0, 0, 0, 0, 0x20, VREG_VOLTAGE_0_80, true}, // 3.48 mW

	// lposc Max frequency (trim)                                        
	{LPOSC, 0, 0, 0, 0, 0, 0, 0, 0xf0, VREG_VOLTAGE_1_10, true}, // 5.43 mW
	{LPOSC, 0, 0, 0, 0, 0, 0, 0, 0xf0, VREG_VOLTAGE_1_00, true}, // 4.53 mW
	{LPOSC, 0, 0, 0, 0, 0, 0, 0, 0xf0, VREG_VOLTAGE_0_90, true}, // 3.90 mW
	{LPOSC, 0, 0, 0, 0, 0, 0, 0, 0xf0, VREG_VOLTAGE_0_80, true}, // 3.49 mW

	// lposc Min frequency (no trim), min voltage                        
	{LPOSC, 0, 0, 0, 0, 0, 0, 0, 0x00, VREG_VOLTAGE_1_10, true}, // 5.43 mW
	{LPOSC, 0, 0, 0, 0, 0, 0, 0, 0x00, VREG_VOLTAGE_1_00, true}, // 4.53 mW
	{LPOSC, 0, 0, 0, 0, 0, 0, 0, 0x00, VREG_VOLTAGE_0_90, true}, // 3.90 mW
	{LPOSC, 0, 0, 0, 0, 0, 0, 0, 0x00, VREG_VOLTAGE_0_80, true}, // 3.49 mW
};
const uint size_configs = sizeof(configs)/sizeof(configs[0]);

