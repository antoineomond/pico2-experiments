#include "experiments.h"

const struct params expes_minimums_rosc[] = {
	// Baseline
	{ROSC, 0, 0, 0, ROSC_DEFAULT_DIVIDER, ROSC_DEFAULT_RANGE, ROSC_DEFAULT_DRIVE_STRENGTH, ROSC_DEFAULT_DRIVE_STRENGTH, 0, VREG_DEFAULT, true},
	
	// Min frequency, default voltage
	{ROSC, 0, 0, 0, ROSC_MAX_DIVIDER, ROSC_MIN_DRIVE_STRENGTH, ROSC_MIN_DRIVE_STRENGTH, ROSC_MIN_DRIVE_STRENGTH, 0, VREG_DEFAULT, true},
	
	// Default frequency, min voltage
	{ROSC, 0, 0, 0, ROSC_DEFAULT_DIVIDER, ROSC_DEFAULT_RANGE, ROSC_DEFAULT_DRIVE_STRENGTH, ROSC_DEFAULT_DRIVE_STRENGTH, 0, VREG_VOLTAGE_0_80, true},
	
	// Min frequency, min voltage
	{ROSC, 0, 0, 0, ROSC_MAX_DIVIDER, ROSC_MIN_DRIVE_STRENGTH, ROSC_MIN_DRIVE_STRENGTH, ROSC_MIN_DRIVE_STRENGTH, 0, VREG_VOLTAGE_0_80, true},
};
const uint size_expes_minimums_rosc = sizeof(expes_minimums_rosc)/sizeof(expes_minimums_rosc[0]);
