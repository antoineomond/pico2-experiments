#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/clocks.h"
#include "hardware/powman.h"

int main() {
	stdio_init_all();
	sleep_ms(1000);
	const uint trims[] = {0x00, 0x10, 0x20, 0x30, 0x3f};
	for (int trim_index = 0; trim_index < sizeof(trims)/sizeof(trims[0]); trim_index++) {
		powman_clear_bits(&powman_hw->lposc, POWMAN_LPOSC_TRIM_BITS);
		powman_set_bits(&powman_hw->lposc, POWMAN_LPOSC_TRIM_BITS & (trims[trim_index] << POWMAN_LPOSC_TRIM_LSB));
		sleep_ms(1000);
		uint lposc_freq = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_LPOSC_CLKSRC)*KHZ;
		printf("trim: 0x%x, lposc frequency: %d Hz\n", trims[trim_index], lposc_freq);
	}
	while(true){
		sleep_ms(60000);
	}
}
