#include "target_configuration.h"

const struct config expes_all[] = {
	// Baseline
	{PLL_SYS, PLL_DEFAULT_VCO_FREQ_HZ, PLL_DEFAULT_POSTDIV1, PLL_DEFAULT_POSTDIV2, 0, 0, 0, 0, 0, VREG_DEFAULT, true},
	{XOSC, 0, 0, 0, 0, 0, 0, 0, 0, VREG_DEFAULT, true},
	{ROSC, 0, 0, 0, ROSC_DEFAULT_DIVIDER, ROSC_DEFAULT_RANGE, ROSC_DEFAULT_DRIVE_STRENGTH, ROSC_DEFAULT_DRIVE_STRENGTH, 0, VREG_DEFAULT, true},
	
	// Baseline
	{PLL_SYS, PLL_DEFAULT_VCO_FREQ_HZ, PLL_DEFAULT_POSTDIV1, PLL_DEFAULT_POSTDIV2, 0, 0, 0, 0, 0, VREG_DEFAULT, true},
	// Min frequency, default voltage
	{PLL_SYS, PLL_MIN_VCO_FREQ_HZ, PLL_MAX_POSTDIV, PLL_MAX_POSTDIV, 0, 0, 0, 0, 0, VREG_DEFAULT, true},
	// Default frequency, min voltage
	{PLL_SYS, PLL_DEFAULT_VCO_FREQ_HZ, PLL_DEFAULT_POSTDIV1, PLL_DEFAULT_POSTDIV2, 0, 0, 0, 0, 0, VREG_MIN_PLL, true},
	// Min frequency, min voltage
	{PLL_SYS, PLL_MIN_VCO_FREQ_HZ, PLL_MAX_POSTDIV, PLL_MAX_POSTDIV, 0, 0, 0, 0, 0, VREG_MIN_PLL, true},
	
	// Baseline
	{ROSC, 0, 0, 0, ROSC_DEFAULT_DIVIDER, ROSC_DEFAULT_RANGE, ROSC_DEFAULT_DRIVE_STRENGTH, ROSC_DEFAULT_DRIVE_STRENGTH, 0, VREG_DEFAULT, true},
	// Min frequency, default voltage
	{ROSC, 0, 0, 0, ROSC_MAX_DIVIDER, ROSC_MIN_RANGE, ROSC_MIN_DRIVE_STRENGTH, ROSC_MIN_DRIVE_STRENGTH, 0, VREG_DEFAULT, true},
	// Default frequency, min voltage
	{ROSC, 0, 0, 0, ROSC_DEFAULT_DIVIDER, ROSC_DEFAULT_RANGE, ROSC_DEFAULT_DRIVE_STRENGTH, ROSC_DEFAULT_DRIVE_STRENGTH, 0, VREG_VOLTAGE_0_80, true},
	// Min frequency, min voltage
	{ROSC, 0, 0, 0, ROSC_MAX_DIVIDER, ROSC_MIN_RANGE, ROSC_MIN_DRIVE_STRENGTH, ROSC_MIN_DRIVE_STRENGTH, 0, VREG_VOLTAGE_0_80, true},
	
	// Min frequency, min voltage
	{XOSC, 0, 0, 0, 0, 0, 0, 0, 0, VREG_VOLTAGE_1_10, true},
	{XOSC, 0, 0, 0, 0, 0, 0, 0, 0, VREG_VOLTAGE_1_00, true},
	{XOSC, 0, 0, 0, 0, 0, 0, 0, 0, VREG_VOLTAGE_0_90, true},
	{XOSC, 0, 0, 0, 0, 0, 0, 0, 0, VREG_VOLTAGE_0_80, true},
	
	// Baseline
	{LPOSC, 0, 0, 0, 0, 0, 0, 0, 0x20, VREG_VOLTAGE_1_10, true},
	{LPOSC, 0, 0, 0, 0, 0, 0, 0, 0x20, VREG_VOLTAGE_1_00, true},
	{LPOSC, 0, 0, 0, 0, 0, 0, 0, 0x20, VREG_VOLTAGE_0_90, true},
	{LPOSC, 0, 0, 0, 0, 0, 0, 0, 0x20, VREG_VOLTAGE_0_80, true},
	
	// Max frequency (trim)
	{LPOSC, 0, 0, 0, 0, 0, 0, 0, 0xf0, VREG_VOLTAGE_1_10, true},
	{LPOSC, 0, 0, 0, 0, 0, 0, 0, 0xf0, VREG_VOLTAGE_1_00, true},
	{LPOSC, 0, 0, 0, 0, 0, 0, 0, 0xf0, VREG_VOLTAGE_0_90, true},
	{LPOSC, 0, 0, 0, 0, 0, 0, 0, 0xf0, VREG_VOLTAGE_0_80, true},

	// Min frequency (no trim), min voltage
	{LPOSC, 0, 0, 0, 0, 0, 0, 0, 0x00, VREG_VOLTAGE_1_10, true},
	{LPOSC, 0, 0, 0, 0, 0, 0, 0, 0x00, VREG_VOLTAGE_1_00, true},
	{LPOSC, 0, 0, 0, 0, 0, 0, 0, 0x00, VREG_VOLTAGE_0_90, true},
	{LPOSC, 0, 0, 0, 0, 0, 0, 0, 0x00, VREG_VOLTAGE_0_80, true}
};
const uint size_expes_all = sizeof(expes_all)/sizeof(expes_all[0]);
