#include "pico/stdlib.h"
#include "pico/sleep.h"
#include <stdio.h>
#include "hardware/watchdog.h"
#include "hardware/clocks.h"
#include "hardware/structs/usb.h"
#include "hardware/powman.h"
// For wfi
#include "hardware/sync.h"
#include "pico/runtime_init.h"

#define VALIDATION_RUN 0
const int expe_pin = 11;
const uint32_t RESET_VAL = 0xDEADBEEF; 
float TIME_RATE = 1;

void pull_down_gpios() {
	const uint used_gpios[] = {expe_pin};
	size_t nb_gpios = sizeof(used_gpios)/sizeof(used_gpios[0]);
	for (int gpio = 0; gpio < NUM_BANK0_GPIOS; gpio++) {
		if(!expe_pin) {
			gpio_set_dir(gpio, 0);
			gpio_set_function(gpio, GPIO_FUNC_SIO);
			if(gpio > NUM_BANK0_GPIOS - NUM_ADC_CHANNELS) {
				gpio_disable_pulls(gpio);
				gpio_set_input_enabled(gpio, false);
			}
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

int main() {
	// Set on/off pin to toggle experiments (also puts the expe_pin to 0)
	// Inspired from https://github.com/peterharperuk/pico-examples/commit/7dccd00d15ded4ddf961f44fdcd1f11a9d8c8be1
	gpio_init(expe_pin);
	gpio_set_dir(expe_pin, GPIO_OUT);
	
	#if VALIDATION_RUN
	stdio_init_all();
	sleep_ms(1000);
	#endif
	#if !VALIDATION_RUN
	// Leave 10sec window to unplug the SWD before resetting the board (required because the SWD sub-system doesn't deactivate automatically once SWD is unplugged (3.5.1. of datasheet))
	if(watchdog_hw->scratch[0] != RESET_VAL) {
		sleep_us((int)(10000000*TIME_RATE)); 
		watchdog_hw->scratch[0] = RESET_VAL;
		watchdog_reboot(0, 0, 0);
	}
	#endif

	set_sys_clock_48mhz(); // run everything from pll_usb and stop pll_sys
	// Disable unused gpios and clocks
	pull_down_gpios();
	turn_off_clocks();
	
	hw_set_bits(&powman_hw->vreg_ctrl, POWMAN_PASSWORD_BITS | POWMAN_VREG_CTRL_UNLOCK_BITS);
	disable_usb();
	powman_set_debug_power_request_ignored(true);
	powman_timer_start();
	uint64_t ms = powman_timer_get_ms();
	powman_enable_alarm_wakeup_at_ms(ms + 30000);
	uint off_state = 0b0000;
	uint on_state = 0b1111;
	bool valid_state = powman_configure_wakeup_state(off_state, on_state);
	if (!valid_state) {
			return 0;
	}
	int rc = powman_set_power_state(off_state);
	if (rc != PICO_OK) {
			return 0;
	}
	gpio_put(expe_pin, 1);
	__wfi();
	return 0; // Should never reach here
}
