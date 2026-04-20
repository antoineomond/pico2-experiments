#include "target_configuration.h"
#include <pico/stdio.h>
#include <stdio.h>
	
extern struct config configs[];
float TIME_RATE = 1;

int main() {
	stdio_init_all();
	sleep_ms(1000);
	printf("test\n");
	const struct config conf = configs[19];
	print_configuration(&conf);
	uint clk_src_freq = switch_configuration_from_parameter(&conf);
	sleep_us((int)(3*1000000*TIME_RATE));
	switch_to_default_configuration();
	printf("clk_freq: %d\n", clk_src_freq);
	while(true) {
		sleep_ms(10000);
	}
}
