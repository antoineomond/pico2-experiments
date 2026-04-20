#include "target_configuration.h"

const struct config expes_minimums_lposc_max_freq[] = {
	// Max frequency (trim)
	{LPOSC, 0, 0, 0, 0, 0, 0, 0, 0xf0, VREG_VOLTAGE_1_10, true},
	{LPOSC, 0, 0, 0, 0, 0, 0, 0, 0xf0, VREG_VOLTAGE_1_00, true},
	{LPOSC, 0, 0, 0, 0, 0, 0, 0, 0xf0, VREG_VOLTAGE_0_90, true},
	{LPOSC, 0, 0, 0, 0, 0, 0, 0, 0xf0, VREG_VOLTAGE_0_80, true}
};
const uint size_expes_minimums_lposc_max_freq = sizeof(expes_minimums_lposc_max_freq)/sizeof(expes_minimums_lposc_max_freq[0]);
