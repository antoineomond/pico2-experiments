#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/xosc.h"
#include "hardware/structs/rosc.h"
#include "hardware/rosc.h"
#include <hardware/gpio.h>
#include <hardware/uart.h>
#include <pico.h>
#include <pico/aon_timer.h>
#include <pico/stdio.h>
#include <pico/time.h>
#include <pico/rand.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hardware/pll.h"
#include "hardware/powman.h"
#include "hardware/regs/powman.h"
#include "pico/sleep.h"
#include "hardware/ticks.h"
#include "experiments.h"

float TIME_RATE = 1;

static void start_all_ticks(void) {
    uint32_t cycles = clock_get_hz(clk_ref) / MHZ;
		if(cycles <= 0) {
			cycles = 1;
		}
    // Note RP2040 has a single tick generator in the watchdog which serves
    // watchdog, system timer and M0+ SysTick; The tick generator is clocked from clk_ref
    // but is now adapted by the hardware_ticks library for compatibility with RP2350
    // npte: hardware_ticks library now provides an adapter for RP2040

    for (int i = 0; i < (int)TICK_COUNT; ++i) {
        tick_start((tick_gen_num_t)i, cycles);
    }
}

static void restart_all_ticks(void) {
	for (int i = 0; i < (int)TICK_COUNT; ++i) {
			tick_stop((tick_gen_num_t)i);
			while(tick_is_running((tick_gen_num_t)i)) tight_loop_contents();
	}
	start_all_ticks();
}

// This function overrides the function defined in runtime_init_clocks.c. It is run before the main() function.
void runtime_init_clocks(void) {
	clocks_hw->resus.ctrl = 0;
	xosc_init();
	
	// Before we touch PLLs, switch sys and ref cleanly away from their aux sources.
	hw_clear_bits(&clocks_hw->clk[clk_sys].ctrl, CLOCKS_CLK_SYS_CTRL_SRC_BITS);
	while (clocks_hw->clk[clk_sys].selected != 0x1)
			tight_loop_contents();
	hw_clear_bits(&clocks_hw->clk[clk_ref].ctrl, CLOCKS_CLK_REF_CTRL_SRC_BITS);
	while (clocks_hw->clk[clk_ref].selected != 0x1)
			tight_loop_contents();

	pll_init(pll_sys, PLL_SYS_REFDIV, PLL_SYS_VCO_FREQ_HZ, PLL_SYS_POSTDIV1, PLL_SYS_POSTDIV2);
	pll_init(pll_usb, PLL_USB_REFDIV, PLL_USB_VCO_FREQ_HZ, PLL_USB_POSTDIV1, PLL_USB_POSTDIV2);

	// Configure clocks

	// RP2040 CLK_REF = XOSC (usually) 12MHz / 1 = 12MHz
	// RP2350 CLK_REF = XOSC (XOSC_MHZ) / N (1,2,4) = 12MHz

	// clk_ref aux select is 0 because:
	//
	// - RP2040: no aux mux on clk_ref, so this field is don't-care.
	//
	// - RP2350: there is an aux mux, but we are selecting one of the
	//   non-aux inputs to the glitchless mux, so the aux select doesn't
	//   matter. The value of 0 here happens to be the sys PLL.
	clock_configure_undivided(clk_ref,
									CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC,
									0,
									XOSC_HZ);

	// CLK SYS = PLL SYS (usually) 125MHz / 1 = 125MHz
	clock_configure_undivided(clk_sys,
									CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX,
									CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS,
									SYS_CLK_HZ);
	/// \end::configure_clk_sys[]

	// CLK USB = PLL USB 48MHz / 1 = 48MHz
	clock_configure_undivided(clk_usb,
									0, // No GLMUX
									CLOCKS_CLK_USB_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB,
									USB_CLK_HZ);

	// CLK ADC = PLL USB 48MHZ / 1 = 48MHz
	clock_configure_undivided(clk_adc,
									0, // No GLMUX
									CLOCKS_CLK_USB_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB,
									USB_CLK_HZ);

	// CLK PERI = clk_sys. Used as reference clock for UART and SPI serial.
	clock_configure_undivided(clk_peri,
									0,
									CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS,
									SYS_CLK_HZ);

	// CLK_HSTX = clk_sys. Transmit bit clock for the HSTX peripheral.
	clock_configure_undivided(clk_hstx,
									0,
									CLOCKS_CLK_HSTX_CTRL_AUXSRC_VALUE_CLK_SYS,
									SYS_CLK_HZ);

	// Deactivate the clocks we don't need
	
	// Finally, all clocks are configured so start the ticks
	// The ticks use clk_ref so now that is configured we can start them
	start_all_ticks();
}

void experiment_pll_sys() {
	
}

void experiment_pll_sys_low_freq() {
	// Temporarily switch from pll to xosc
	clock_configure_undivided(clk_sys,
									CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX,
									CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_XOSC_CLKSRC,
									XOSC_HZ);
	clock_configure_undivided(clk_usb,
									0, // No GLMUX
									CLOCKS_CLK_USB_CTRL_AUXSRC_VALUE_XOSC_CLKSRC,
									USB_CLK_HZ);
	clock_configure_undivided(clk_adc,
									0, // No GLMUX
									CLOCKS_CLK_USB_CTRL_AUXSRC_VALUE_XOSC_CLKSRC,
									USB_CLK_HZ);
	sleep_ms(100);
	
	// change pll frequency
	pll_deinit(pll_usb);
	pll_deinit(pll_sys);
	// With a 12MHz xosc, vco: min 756MHz and max 1596MHz
	uint postdiv1_sys = 6;
	uint postdiv2_sys = 1;
	uint postdiv1_usb = 4;
	uint postdiv2_usb = 4;
	pll_init(pll_sys, PLL_SYS_REFDIV, 900*MHZ, postdiv1_sys, postdiv2_sys);
	pll_init(pll_usb, PLL_USB_REFDIV, 768*MHZ, postdiv1_usb, postdiv2_usb);
	sleep_ms(100);
	
	// Reassign pll to the clocks generator
	clock_configure_undivided(clk_sys,
									CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX,
									CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS,
									SYS_CLK_HZ);
	clock_configure_undivided(clk_usb,
									0, // No GLMUX
									CLOCKS_CLK_USB_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB,
									USB_CLK_HZ);
	clock_configure_undivided(clk_adc,
									0, // No GLMUX
									CLOCKS_CLK_USB_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB,
									USB_CLK_HZ);
}

void experiment_pll_sys_high_freq() {
	// Temporarily switch from pll to xosc
	clock_configure_undivided(clk_sys,
									CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX,
									CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_XOSC_CLKSRC,
									XOSC_HZ);
	clock_configure_undivided(clk_usb,
									0, // No GLMUX
									CLOCKS_CLK_USB_CTRL_AUXSRC_VALUE_XOSC_CLKSRC,
									USB_CLK_HZ);
	clock_configure_undivided(clk_adc,
									0, // No GLMUX
									CLOCKS_CLK_USB_CTRL_AUXSRC_VALUE_XOSC_CLKSRC,
									USB_CLK_HZ);
	sleep_ms(100);
	
	// change pll frequency
	pll_deinit(pll_usb);
	pll_deinit(pll_sys);
	// With a 12MHz xosc, vco: min 756MHz and max 1596MHz
	uint postdiv1_sys = 5;
	uint postdiv2_sys = 2;
	uint postdiv1_usb = 6;
	uint postdiv2_usb = 5;
	pll_init(pll_sys, PLL_SYS_REFDIV, 1500*MHZ, postdiv1_sys, postdiv2_sys);
	pll_init(pll_usb, PLL_USB_REFDIV, 1440*MHZ, postdiv1_usb, postdiv2_usb);
	sleep_ms(100);
	
	// Reassign pll to the clocks generator
	clock_configure_undivided(clk_sys,
									CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX,
									CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS,
									SYS_CLK_HZ);
	clock_configure_undivided(clk_usb,
									0, // No GLMUX
									CLOCKS_CLK_USB_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB,
									USB_CLK_HZ);
	clock_configure_undivided(clk_adc,
									0, // No GLMUX
									CLOCKS_CLK_USB_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB,
									USB_CLK_HZ);
	
}

void experiment_xosc_sys() {
	clock_configure_undivided(clk_sys,
									CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX,
									CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_XOSC_CLKSRC,
									XOSC_HZ);
}

void experiment_xosc_sys_no_pll() {
	pll_deinit(pll_usb);
	pll_deinit(pll_sys);
}


void experiment_rosc_sys_ref() {
	uint f = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_ROSC_CLKSRC)*1000;
	clock_configure_undivided(clk_ref,
									CLOCKS_CLK_REF_CTRL_SRC_VALUE_ROSC_CLKSRC_PH,
									0,
									f);  // Need to be more precise on this value
	clock_configure_undivided(clk_sys,
									CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX,
									CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_ROSC_CLKSRC,
									f); // Need to be more precise on this value
	restart_all_ticks();  // clk_ref changed, need to re-compute all ticks
}

void experiment_rosc_sys_ref_no_xosc() {
	xosc_disable();
}

void experiment_lposc_sys_ref() {
	//uint lposc_freq = 32768;
	uint lposc_freq = 29000;
	uint nb_cycles_threshold = 1000000;
	TIME_RATE = (float)lposc_freq/(float)nb_cycles_threshold; // LPOSC isn't fast enough to generate the 1us tick (hardwired value). The TIME_RATE divides any timing to account for this slowness 
	clock_configure_undivided(clk_ref,
									CLOCKS_CLK_REF_CTRL_SRC_VALUE_LPOSC_CLKSRC,
									0,
									lposc_freq);  // Need to be more precise on this value
	clock_configure_undivided(clk_sys,
									CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLK_REF,
									CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_ROSC_CLKSRC,
									lposc_freq); // Need to be more precise on this value
	restart_all_ticks();  // clk_ref changed, need to re-compute all ticks
}

void experiment_lposc_sys_ref_no_rosc() {
	rosc_disable();
}
