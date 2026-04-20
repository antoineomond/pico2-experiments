#include "target_configuration.h"
#include <pico/stdio.h>
#include <stdio.h>
	
float TIME_RATE = 1;

int main() {
	stdio_init_all();
	sleep_ms(1000);
	const struct config conf = configs[19];
	print_configuration(&conf);
	uint clk_src_freq = switch_configuration_from_parameter(&conf);
	sleep_us((int)(3*1000000*TIME_RATE));
	switch_to_default_configuration();
	printf("measured clock frequency: %dHz\n", clk_src_freq);
	while(true) {
		sleep_ms(10000);
	}
}
