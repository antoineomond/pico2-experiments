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

#define LOW_VREG_EXPES 0
#define BUFF_LEN 6000
#define LINE_SIZE 40 

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

static int index_strings = 0;
static char (*strings_buffer)[LINE_SIZE];

void led_blink(uint count) {
	gpio_init(PICO_DEFAULT_LED_PIN);
	gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
	for (int i = 0; i < count; i++) {
		gpio_put(PICO_DEFAULT_LED_PIN, 1);
		sleep_us((int)(250000));
		gpio_put(PICO_DEFAULT_LED_PIN, 0);
		sleep_us((int)(250000));
	}
}

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
	strings_buffer = malloc(BUFF_LEN * sizeof(*strings_buffer));
	//const uint ranges[] = {ROSC_CTRL_FREQ_RANGE_VALUE_LOW, ROSC_CTRL_FREQ_RANGE_VALUE_MEDIUM, ROSC_CTRL_FREQ_RANGE_VALUE_HIGH};
	//const uint codes[4] = {
	//	0x00000007, 0x00000077, 0x00000777, 0x00007777,
	//};
	//#if LOW_VREG_EXPES
	//const uint vregs[3] = {
	//	VREG_VOLTAGE_0_85, VREG_VOLTAGE_0_80, VREG_VOLTAGE_0_75
	//};
	//turn_off_plls();
	//#else
	//const uint vregs[5] = {
	//	VREG_VOLTAGE_1_10, VREG_VOLTAGE_1_05, VREG_VOLTAGE_1_00, VREG_VOLTAGE_0_95,
	//	VREG_VOLTAGE_0_90
	//};
	
	const uint vregs[] = {VREG_VOLTAGE_0_85};
	turn_off_plls();
	//stdio_init_all();
	//sleep_ms(1000);
	//#endif
	
	vreg_disable_voltage_limit();
	powman_clear_bits(&powman_hw->bod, 0x000001f1);
	
	//rosc_set_div(17);
	//rosc_set_range(0xfa6);
	//rosc_write(&rosc_hw->freqa, (ROSC_FREQA_PASSWD_VALUE_PASS << ROSC_FREQA_PASSWD_LSB) | 0x0377);
	//rosc_write(&rosc_hw->freqb, (ROSC_FREQA_PASSWD_VALUE_PASS << ROSC_FREQA_PASSWD_LSB) | 0x0007);
	//sleep_ms(100);
	//
	//uint rosc_freq = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_ROSC_CLKSRC);
	//printf("rosc_freq: %d\n", rosc_freq);
	//
	//while(true){
	//	sleep_ms(1000);
	//}
	//printf("iteration,clock_source,vreg,divider,range,freqa,freqb,clock_freq\n");
	const uint nb_iterations = 20;
	const uint max_divider = 6;
	const uint ranges[] = {ROSC_CTRL_FREQ_RANGE_VALUE_LOW, ROSC_CTRL_FREQ_RANGE_VALUE_MEDIUM, ROSC_CTRL_FREQ_RANGE_VALUE_HIGH};
	const uint codes[16] = {
		0x00000001, 0x00000003, 0x00000005, 0x00000007,
		0x00000017, 0x00000037, 0x00000057, 0x00000077,
		0x00000177, 0x00000377, 0x00000577, 0x00000777,
		0x00001777, 0x00003777, 0x00005777, 0x00007777,
	};
	for (int iteration = 0; iteration < nb_iterations; iteration++) {
		for (int vreg_index = 0; vreg_index < sizeof(vregs)/sizeof(vregs[0]); vreg_index++) {
			vreg_set_voltage(vregs[vreg_index]);
			for (int divider = 1; divider < max_divider; divider++) {
				rosc_set_div(divider);
				for (int range_index = 0; range_index < sizeof(ranges)/sizeof(ranges[0]); range_index++) {
					rosc_set_range(ranges[range_index]);
					for (int freqa_code_index = 0; freqa_code_index < sizeof(codes)/sizeof(codes[0]); freqa_code_index++) {
						rosc_write(&rosc_hw->freqa, (ROSC_FREQA_PASSWD_VALUE_PASS << ROSC_FREQA_PASSWD_LSB) | codes[freqa_code_index]);
						for (int freqb_code_index = 0; freqb_code_index < sizeof(codes)/sizeof(codes[0]); freqb_code_index++) {
							rosc_write(&rosc_hw->freqb, (ROSC_FREQA_PASSWD_VALUE_PASS << ROSC_FREQA_PASSWD_LSB) | codes[freqb_code_index]);
							uint rosc_freq = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_ROSC_CLKSRC);
							//#if LOW_VREG_EXPES
							sprintf(strings_buffer[index_strings++], "%d,rosc,%.2d,%.2d,%x,%.4x,%.4x,%d\n", iteration, vregs[vreg_index], divider, ranges[range_index], codes[freqa_code_index], codes[freqb_code_index], rosc_freq);

							if(index_strings >= BUFF_LEN) {
								printf_buffer();
								vreg_set_voltage(vregs[vreg_index]);
							}
							//#else
							//printf("%d,rosc,%.2d,%.2d,%x,%.4x,%.4x,%d\n", iteration, vregs[vreg_index], divider, ranges[range_index], codes[freqa_code_index], codes[freqb_code_index], rosc_freq);
							//#endif
						}
					}
				}
			}
		}
	}
	vreg_set_voltage(VREG_VOLTAGE_1_10);
	//#if LOW_VREG_EXPES
	printf_buffer();
	//#endif
	
	while(true){
		sleep_ms(1000);
	}
}
