#include "pico/stdlib.h"
#include <stdlib.h>
#include <pico/stdio.h>
#include <stdio.h>
#include <pico/time.h>
#include "hardware/rosc.h"
#include "hardware/clocks.h"
#include "hardware/vreg.h"
#include "hardware/pll.h"
#include "hardware/powman.h"
#include <string.h>

#define BUFF_LEN 500
#define LINE_SIZE 50

void turn_off_plls() {
	clock_configure_undivided(clk_sys,
									CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX,
									CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_XOSC_CLKSRC,
									SYS_CLK_HZ);

	clock_configure_undivided(clk_usb,
									0, // No GLMUX
									CLOCKS_CLK_USB_CTRL_AUXSRC_VALUE_XOSC_CLKSRC,
									USB_CLK_HZ);

	pll_deinit(pll_sys);
	pll_deinit(pll_usb);
}

void turn_on_plls() {
	vreg_set_voltage(VREG_VOLTAGE_1_10);
	sleep_ms(100);
	
	pll_init(pll_sys, PLL_SYS_REFDIV, PLL_SYS_VCO_FREQ_HZ, PLL_SYS_POSTDIV1, PLL_SYS_POSTDIV2);
	pll_init(pll_usb, PLL_USB_REFDIV, PLL_USB_VCO_FREQ_HZ, PLL_USB_POSTDIV1, PLL_USB_POSTDIV2);

	clock_configure_undivided(clk_sys,
									CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX,
									CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS,
									SYS_CLK_HZ);

	clock_configure_undivided(clk_usb,
									0, // No GLMUX
									CLOCKS_CLK_USB_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB,
									USB_CLK_HZ);
}

int index_strings = 0;
char (*strings_buffer)[BUFF_LEN];

void printf_buffer() {
	turn_on_plls();
	
	stdio_init_all();
	sleep_ms(1000);
	
	for (int i = 0; i < index_strings; i++) {
		printf(strings_buffer[i]);
	}
	
	// Everything has been printed, reset the index_strings for new data
	stdio_deinit_all();
	index_strings = 0;

	turn_off_plls();
}

int main() {
	strings_buffer = malloc(sizeof(char[LINE_SIZE][BUFF_LEN]));
	const uint trims[5] = {
		0x000, 0x100, 0x200, 0x300, 0x3f0,
	};
	const uint vregs[8] = {
		VREG_VOLTAGE_1_10, VREG_VOLTAGE_1_05, VREG_VOLTAGE_1_00, VREG_VOLTAGE_0_95, VREG_VOLTAGE_0_90, VREG_VOLTAGE_0_85, VREG_VOLTAGE_0_80, VREG_VOLTAGE_0_75
	};
	turn_off_plls();
	
	vreg_disable_voltage_limit();
	powman_clear_bits(&powman_hw->bod, 0x000001f1);
	
	printf("clock_source,vreg,trim,clock_freq\n");
	for (int vreg_index = 0; vreg_index < sizeof(vregs)/sizeof(vregs[0]); vreg_index++) {
		for (int trim_index = 0; trim_index < sizeof(trims)/sizeof(trims[0]); trim_index++) {
			vreg_set_voltage(vregs[vreg_index]);
			sleep_ms(100);
			
			powman_clear_bits(&powman_hw->lposc, POWMAN_LPOSC_TRIM_BITS);
			powman_set_bits(&powman_hw->lposc, POWMAN_LPOSC_TRIM_BITS & trims[trim_index]);
			uint lposc_freq = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_LPOSC_CLKSRC)*KHZ;
			sleep_ms(100);
			
			sprintf(strings_buffer[index_strings++], "lposc,%.2d,%.3x,%d\n", vregs[vreg_index], trims[trim_index], lposc_freq);
			if(index_strings >= BUFF_LEN) {
				printf_buffer();
			}
		}
	}
	printf_buffer();
	
	while(true){
		sleep_ms(1000);
	}
}
