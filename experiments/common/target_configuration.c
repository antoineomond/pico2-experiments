#include "experiments.h"
#include "pico/stdlib.h"
#include <stdlib.h>
#include <stdio.h>
#include "pico/multicore.h"
#include "hardware/clocks.h"
#include "hardware/structs/usb.h"
#include "hardware/ticks.h"
#include "hardware/pll.h"
#include "hardware/xosc.h"
#include "hardware/rosc.h"
#include "hardware/watchdog.h"
#include "hardware/vreg.h"
#include "hardware/powman.h"

struct result {
	params p;
	float power_median_mw;
};

void pull_down_gpios() {
	for (int gpio = 0; gpio < NUM_BANK0_GPIOS; gpio++) {
		gpio_set_dir(gpio, 0);
		gpio_set_function(gpio, GPIO_FUNC_SIO);
		if(gpio > NUM_BANK0_GPIOS - NUM_ADC_CHANNELS) {
			gpio_disable_pulls(gpio);
			gpio_set_input_enabled(gpio, false);
		}
	}
}

void turn_off_clocks() {
	clock_hw_t *clock_hw = &clocks_hw->clk[clk_usb];
	hw_clear_bits(&clock_hw->ctrl, CLOCKS_CLK_USB_CTRL_ENABLE_BITS);
	clock_hw = &clocks_hw->clk[clk_adc];
	hw_clear_bits(&clock_hw->ctrl, CLOCKS_CLK_ADC_CTRL_ENABLE_BITS);
	clock_hw = &clocks_hw->clk[clk_hstx];
	hw_clear_bits(&clock_hw->ctrl, CLOCKS_CLK_HSTX_CTRL_ENABLE_BITS);
}

static void disable_usb() {
    usb_hw->phy_direct = USB_USBPHY_DIRECT_TX_PD_BITS | USB_USBPHY_DIRECT_RX_PD_BITS | USB_USBPHY_DIRECT_DM_PULLDN_EN_BITS | USB_USBPHY_DIRECT_DP_PULLDN_EN_BITS;
    
    usb_hw->phy_direct_override = USB_USBPHY_DIRECT_RX_DM_BITS | USB_USBPHY_DIRECT_RX_DP_BITS |          USB_USBPHY_DIRECT_RX_DD_BITS |
        USB_USBPHY_DIRECT_OVERRIDE_TX_DIFFMODE_OVERRIDE_EN_BITS | USB_USBPHY_DIRECT_OVERRIDE_DM_PULLUP_OVERRIDE_EN_BITS | USB_USBPHY_DIRECT_OVERRIDE_TX_FSSLEW_OVERRIDE_EN_BITS |
        USB_USBPHY_DIRECT_OVERRIDE_TX_PD_OVERRIDE_EN_BITS | USB_USBPHY_DIRECT_OVERRIDE_RX_PD_OVERRIDE_EN_BITS | USB_USBPHY_DIRECT_OVERRIDE_TX_DM_OVERRIDE_EN_BITS |
        USB_USBPHY_DIRECT_OVERRIDE_TX_DP_OVERRIDE_EN_BITS | USB_USBPHY_DIRECT_OVERRIDE_TX_DM_OE_OVERRIDE_EN_BITS | USB_USBPHY_DIRECT_OVERRIDE_TX_DP_OE_OVERRIDE_EN_BITS |
        USB_USBPHY_DIRECT_OVERRIDE_DM_PULLDN_EN_OVERRIDE_EN_BITS | USB_USBPHY_DIRECT_OVERRIDE_DP_PULLDN_EN_OVERRIDE_EN_BITS | USB_USBPHY_DIRECT_OVERRIDE_DP_PULLUP_EN_OVERRIDE_EN_BITS |
        USB_USBPHY_DIRECT_OVERRIDE_DM_PULLUP_HISEL_OVERRIDE_EN_BITS | USB_USBPHY_DIRECT_OVERRIDE_DP_PULLUP_HISEL_OVERRIDE_EN_BITS;
}

static void sleep_callback(void) {}

void processor_deep_sleep(void) {
		// From pico-extras
    // Enable deep sleep at the proc
#ifdef __riscv
    uint32_t bits = RVCSR_MSLEEP_POWERDOWN_BITS;
    if (!get_core_num()) {
        bits |= RVCSR_MSLEEP_DEEPSLEEP_BITS;
    }
    riscv_set_csr(RVCSR_MSLEEP_OFFSET, bits);
#else
    scb_hw->scr |= ARM_CPU_PREFIXED(SCR_SLEEPDEEP_BITS);
#endif
}

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

void restart_all_ticks(void) {
	for (int i = 0; i < (int)TICK_COUNT; ++i) {
			tick_stop((tick_gen_num_t)i);
			while(tick_is_running((tick_gen_num_t)i)) tight_loop_contents();
	}
	start_all_ticks();
}

uint leverage_clock_source_lposc(uint trim) {
	// lposc has to be clk_ref
	// Put xosc as clk_ref to count lposc frequency
	xosc_init();
	clock_configure_undivided(clk_ref, CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC, 0, XOSC_HZ);
	restart_all_ticks();
	
	// Specify lposc frequency
	powman_clear_bits(&powman_hw->lposc, POWMAN_LPOSC_TRIM_BITS);
	powman_set_bits(&powman_hw->lposc, POWMAN_LPOSC_TRIM_BITS & (trim << POWMAN_LPOSC_TRIM_LSB));
	sleep_ms(100);
	
	uint clk_src_freq = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_LPOSC_CLKSRC) * KHZ;
	
	clock_configure_undivided(clk_ref, CLOCKS_CLK_REF_CTRL_SRC_VALUE_LPOSC_CLKSRC, 0, clk_src_freq);
	restart_all_ticks();
	TIME_RATE = ((float)clk_src_freq)/((float)1*MHZ); // LPOSC isn't fast enough to generate the 1us tick (hardwired value). The TIME_RATE divides any active wait to account for this slowness
	clock_set_reported_hz(clk_sys, clk_src_freq);
	
	// Disable unused clock sources
	pll_deinit(pll_sys);
	pll_deinit(pll_usb);
	xosc_disable();
	rosc_disable();
	
	return clk_src_freq;
}

uint leverage_clock_source_rosc(uint div, uint range, uint freqa, uint freqb) {
	rosc_enable();
	
	// Put xosc as clk_ref to count rosc frequency
	xosc_init();
	clock_configure_undivided(clk_ref, CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC, 0, XOSC_HZ);
	restart_all_ticks();
	
	// Specify rosc frequency
	rosc_set_div(div);
	rosc_set_range(range);
	rosc_write(&rosc_hw->freqa, (ROSC_FREQA_PASSWD_VALUE_PASS << ROSC_FREQA_PASSWD_LSB) | freqa);
	rosc_write(&rosc_hw->freqb, (ROSC_FREQA_PASSWD_VALUE_PASS << ROSC_FREQA_PASSWD_LSB) | freqb);
	sleep_ms(100);
	
	uint clk_src_freq = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_ROSC_CLKSRC_PH) * KHZ;
	
	clock_configure_undivided(clk_sys, CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX, CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_ROSC_CLKSRC, clk_src_freq); // clk_freq to set later in the code
	clock_configure_undivided(clk_ref, CLOCKS_CLK_REF_CTRL_SRC_VALUE_ROSC_CLKSRC_PH, 0, clk_src_freq);
	restart_all_ticks();
	uint divider = ((float)(clk_src_freq/MHZ));
	if(divider == 0) {
		divider = 1;
	}
	TIME_RATE = ((float)((float)clk_src_freq/(float)MHZ))/divider; // clk_ref takes clock_freq/MHz as reference to compute time, trimming all remaining KHz. This leads to incorrect time tracking  
	
	// Disable unused clock sources
	pll_deinit(pll_sys);
	pll_deinit(pll_usb);
	xosc_disable();
	
	return clk_src_freq;
}

uint leverage_clock_source_xosc() {
	xosc_init();
	clock_configure_undivided(clk_ref, CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC, 0, XOSC_HZ);
	restart_all_ticks();
	clock_configure_undivided(clk_sys, CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLK_REF, 0, XOSC_HZ);
	
	uint clk_src_freq = XOSC_HZ; // Known fix frequency
	
	// Disable unused clock sources
	pll_deinit(pll_sys);
	pll_deinit(pll_usb);
	rosc_disable();
	
	return clk_src_freq;
}

uint leverage_clock_source_pll(uint vco_freq, uint div1, uint div2) {
	leverage_clock_source_xosc();
	pll_init(pll_sys, PLL_SYS_REFDIV, vco_freq, div1, div2);
	
	uint clk_src_freq = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_PLL_SYS_CLKSRC_PRIMARY) * KHZ;
	
	clock_configure_undivided(clk_sys, CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX, CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS, clk_src_freq);
	
	// Disable unused clock sources
	pll_deinit(pll_usb);
	rosc_disable();
	
	return clk_src_freq;
}


uint switch_configuration_from_parameter(params p) {
	// Set the voltage, clock source and frequency (measure the frequency for rosc and lposc)
	// pll must be deactivated to reach vreg outputs below 0.9V
	leverage_clock_source_xosc();
	sleep_ms(100);
	vreg_set_voltage(p.vreg_output);
	sleep_ms(100);
	
	// Set clock source with default frequencies
	uint clk_src_freq;
	if(p.clock_source == PLL_SYS) {
		clk_src_freq = leverage_clock_source_pll(p.pll_vco_freq, p.pll_div1, p.pll_div2);
	}
	if(p.clock_source == XOSC) {
		clk_src_freq = leverage_clock_source_xosc();
	}
	if(p.clock_source == ROSC) {
		clk_src_freq = leverage_clock_source_rosc(p.rosc_div, p.rosc_range, p.rosc_drive_freqa, p.rosc_drive_freqb);
	}
	if(p.clock_source == LPOSC) {
		clk_src_freq = leverage_clock_source_lposc(p.lposc_trim);
	}
	return clk_src_freq;
}

void switch_configuration(uint target_power_median_mw) {
	// Search closest configuration that makes the rpi use a power median lower than target_power_median_mw 
	result param = NULL;
	for (uint i = 0; i < size_parameters; i++) {
		if(parameters[i].power_median_mw <= target_power_median_mw) {
			if(param == NULL) {
				param = parameters[i];
			}
			else if (param.power_median_mw <= parameters[i].power_median_mw) {
				param = parameters[i];
			}
		}
	}
	switch_configuration_from_parameter(param.p);
}
