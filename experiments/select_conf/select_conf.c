#include "target_configuration.h"
#include <pico/stdio.h>
#include <stdio.h>
	
extern struct config configs[];
float TIME_RATE = 1;

int main() {
	stdio_init_all();
	sleep_ms(1000);
	printf("test\n");
	const struct config conf = configs[3];
	uint clk_src_freq = switch_configuration_from_parameter(&conf);
	sleep_us((int)(3*1000000*TIME_RATE));
	switch_to_default_configuration();
	printf("%d,%d,%d,%d,%d,%d,%d,%d,0x%.3x,%d,%b\n", conf.clock_source, conf.pll_vco_freq, conf.pll_div1, conf.pll_div2, conf.rosc_div, conf.rosc_range, conf.rosc_drive_freqa, conf.rosc_drive_freqb, conf.lposc_trim, conf.vreg_output, conf.set_as_ref);
	printf("clk_freq: %d\n", clk_src_freq);
	while(true) {
		sleep_ms(10000);
	}
}
