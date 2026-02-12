#include "pico/stdlib.h"
#include <stdlib.h>
#include <pico/stdio.h>
#include <stdio.h>
#include <pico/time.h>
#include "hardware/clocks.h"
#include "hardware/pll.h"
#include <string.h>

int main() {
	stdio_uart_init();
	sleep_ms(1000);
	
	// Stop clocks using pll_usb
	clock_stop(clk_usb);
	clock_stop(clk_adc);
	
	printf("clock_source,vco_freq,div1,div2,clock_freq\n");
	for (int vco_freq = 760; vco_freq < 1570; vco_freq += 10) {
		for (int div1 = 1; div1 < 8; div1++) {
			for (int div2 = 1; div2 < 8; div2++) {
				if(vco_freq/div1/div2 > 150) continue; // Skip all too large freqs
				pll_init(pll_usb, PLL_USB_REFDIV, vco_freq*MHZ, div1, div2);
				sleep_ms(100);
				uint clock_freq = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_PLL_USB_CLKSRC_PRIMARY);
				printf("pll,%.4d,%d,%d,%d\n", vco_freq, div1, div2, clock_freq);
			}
		}
	}
	
	while(true){
		sleep_ms(100000);
	}
}
