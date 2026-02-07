#include "pico/stdlib.h"
#include <pico/stdio.h>
#include <stdio.h>
#include <pico/time.h>
#include "hardware/rosc.h"
#include "hardware/clocks.h"
#include "hardware/vreg.h"
#include "hardware/pll.h"
#include "hardware/powman.h"

// Deactivate PLLs
//clock_configure_undivided(clk_sys,
//								CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX,
//								CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_XOSC_CLKSRC,
//								SYS_CLK_HZ);
//
//clock_configure_undivided(clk_usb,
//								0, // No GLMUX
//								CLOCKS_CLK_USB_CTRL_AUXSRC_VALUE_XOSC_CLKSRC,
//								USB_CLK_HZ);
//
//pll_deinit(pll_sys);
//pll_deinit(pll_usb);


// Activate PLLs
//pll_init(pll_sys, PLL_SYS_REFDIV, PLL_SYS_VCO_FREQ_HZ, PLL_SYS_POSTDIV1, PLL_SYS_POSTDIV2);
//pll_init(pll_usb, PLL_USB_REFDIV, PLL_USB_VCO_FREQ_HZ, PLL_USB_POSTDIV1, PLL_USB_POSTDIV2);
//
//clock_configure_undivided(clk_sys,
//								CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX,
//								CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS,
//								SYS_CLK_HZ);
//
//clock_configure_undivided(clk_usb,
//								0, // No GLMUX
//								CLOCKS_CLK_USB_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB,
//								USB_CLK_HZ);

// Manual testing
//vreg_set_voltage(VREG_VOLTAGE_0_90);
//
//int divider = 1;
//int range_index = 3;
//int freqa_code_index = 13;
//int freqb_code_index = 15;
//int e = 0;
//rosc_set_div(divider);
//rosc_set_range(ranges[range_index]);
//rosc_write(&rosc_hw->freqa, (ROSC_FREQA_PASSWD_VALUE_PASS << ROSC_FREQA_PASSWD_LSB) | codes[freqa_code_index]);
//rosc_write(&rosc_hw->freqb, (ROSC_FREQA_PASSWD_VALUE_PASS << ROSC_FREQA_PASSWD_LSB) | codes[freqb_code_index]);
//sleep_ms(100);
//
//uint rosc_freq = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_ROSC_CLKSRC);
//
//vreg_set_voltage(VREG_VOLTAGE_1_10);
//sleep_ms(100);
//
//
//stdio_init_all();
//sleep_ms(1000);
//
//printf("%.2d,%d,%.4x,%.4x,%d\n", divider, range_index, codes[freqa_code_index], codes[freqb_code_index], rosc_freq);

int main() {
	stdio_init_all();
	sleep_ms(1000);
	const uint ranges[4] = {ROSC_CTRL_FREQ_RANGE_VALUE_LOW, ROSC_CTRL_FREQ_RANGE_VALUE_MEDIUM, ROSC_CTRL_FREQ_RANGE_VALUE_HIGH, ROSC_CTRL_FREQ_RANGE_VALUE_TOOHIGH};
	const uint codes[16] = {
		0x00000001, 0x00000003, 0x00000005, 0x00000007,
		0x00000017, 0x00000037, 0x00000057, 0x00000077,
		0x00000177, 0x00000377, 0x00000577, 0x00000777,
		0x00001777, 0x00003777, 0x00005777, 0x00007777,
	};
	const uint vregs[5] = {
		VREG_VOLTAGE_1_10, VREG_VOLTAGE_1_05, VREG_VOLTAGE_1_00, VREG_VOLTAGE_0_95,
		VREG_VOLTAGE_0_90
	};
	
	vreg_disable_voltage_limit();
	powman_clear_bits(&powman_hw->bod, 0x000001f1);
	
	printf("vreg,divider,range,freqa,freqb,clock_freq\n");
	for (int vreg_index = 0; vreg_index < 5; vreg_index++) {
		for (int divider = 1; divider < 31; divider++) {
			for (int range_index = 0; range_index < 4; range_index++) {
				for (int freqa_code_index = 0; freqa_code_index < 16; freqa_code_index++) {
					for (int freqb_code_index = 0; freqb_code_index < 16; freqb_code_index++) {
						vreg_set_voltage(vregs[vreg_index]);
						rosc_set_div(divider);
						rosc_set_range(ranges[range_index]);
						rosc_write(&rosc_hw->freqa, (ROSC_FREQA_PASSWD_VALUE_PASS << ROSC_FREQA_PASSWD_LSB) | codes[freqa_code_index]);
						rosc_write(&rosc_hw->freqb, (ROSC_FREQA_PASSWD_VALUE_PASS << ROSC_FREQA_PASSWD_LSB) | codes[freqb_code_index]);
						sleep_ms(10);
						
						uint rosc_freq = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_ROSC_CLKSRC);
						printf("%.2d,%.2d,%x,%.4x,%.4x,%d\n", vregs[vreg_index], divider, ranges[range_index], codes[freqa_code_index], codes[freqb_code_index], rosc_freq);
					}
				}
			}
		}
	}
	
	while(true){
		sleep_ms(1000);
	}
}
