#include "target_configuration.h"
#include <pico/stdio.h>
#include <stdio.h>

float TIME_RATE = 1;

int main() {
	struct result* res;
	res = switch_configuration(15);
	sleep_us((int)(3*1000000*TIME_RATE));
	switch_to_default_configuration();
	printf("%d,%d,%d,%d,%d,%d,%d,%d,0x%.3x,%d,%b,%.2f\n", res->p.clock_source, res->p.pll_vco_freq, res->p.pll_div1, res->p.pll_div2, res->p.rosc_div, res->p.rosc_range, res->p.rosc_drive_freqa, res->p.rosc_drive_freqb, res->p.lposc_trim, res->p.vreg_output, res->p.set_as_ref, res->power_median_mw);
	while(true) {
		sleep_ms(10000);
	}
}
