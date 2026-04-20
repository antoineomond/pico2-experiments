#include "target_configuration.h"

const struct config expes_baseline[] = {
	// Baseline
	{PLL_SYS, PLL_DEFAULT_VCO_FREQ_HZ, PLL_DEFAULT_POSTDIV1, PLL_DEFAULT_POSTDIV2, 0, 0, 0, 0, 0, VREG_DEFAULT, true},
	{XOSC, 0, 0, 0, 0, 0, 0, 0, 0, VREG_DEFAULT, true},
	{ROSC, 0, 0, 0, ROSC_DEFAULT_DIVIDER, ROSC_DEFAULT_RANGE, ROSC_DEFAULT_DRIVE_STRENGTH, ROSC_DEFAULT_DRIVE_STRENGTH, 0, VREG_DEFAULT, true},
	//{LPOSC, 0, 0, 0, 0, 0, 0, 0, LPOSC_DEFAULT_TRIM, VREG_DEFAULT},
};
const uint size_expes_baseline = sizeof(expes_baseline)/sizeof(expes_baseline[0]);
