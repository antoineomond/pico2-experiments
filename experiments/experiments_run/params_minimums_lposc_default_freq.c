#include "target_configuration.h"

const struct params expes_minimums_lposc_default_freq[] = {
	// Baseline
	{LPOSC, 0, 0, 0, 0, 0, 0, 0, 0x20, VREG_VOLTAGE_1_10, true},
	
	// Min frequency (no trim), min voltage
	{LPOSC, 0, 0, 0, 0, 0, 0, 0, 0x20, VREG_VOLTAGE_1_00, true},
	{LPOSC, 0, 0, 0, 0, 0, 0, 0, 0x20, VREG_VOLTAGE_0_90, true},
	{LPOSC, 0, 0, 0, 0, 0, 0, 0, 0x20, VREG_VOLTAGE_0_80, true}
};
const uint size_expes_minimums_lposc_default_freq = sizeof(expes_minimums_lposc_default_freq)/sizeof(expes_minimums_lposc_default_freq[0]);
