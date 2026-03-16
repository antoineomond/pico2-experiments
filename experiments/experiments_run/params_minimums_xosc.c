#include "experiments.h"

const struct params expes_minimums_xosc[] = {
	// Min frequency, min voltage
	{XOSC, 0, 0, 0, 0, 0, 0, 0, 0, VREG_VOLTAGE_0_80, true},
};
const uint size_expes_minimums_xosc = sizeof(expes_minimums_xosc)/sizeof(expes_minimums_xosc[0]);
