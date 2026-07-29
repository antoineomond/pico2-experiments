#include <pico/stdio.h>
#include <stdint.h>
#include <stdio.h>
#include "experiments.h"
#include "target_configuration.h"
#include "hardware/structs/usb.h"
#include "pico/time.h"
#include "pico/sync.h"
#include "hardware/timer.h"

const int expe_pin = 11;
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

uint compute_primes_local(uint start, uint end) {
	volatile uint cpt = 0;
	uint is_prime = 1;
	for (int k = start; k < end; k++) {
		is_prime = 1;
		for (int i = 2; i	< k; i++) {
			if (k%i==0) {
				is_prime = 0;
			}	
		}
		if(is_prime == 1) {
			cpt += 1;
		}
	}
	return cpt;
}

void timer_busy() {
	timer_busy_wait_until(PICO_DEFAULT_TIMER_INSTANCE(), make_timeout_time_us(10000000));
}

void busy() {
	while(true) {};
}

int main() {
	gpio_init(expe_pin);
	gpio_set_dir(expe_pin, GPIO_OUT);
	while(true) {
		gpio_put(expe_pin, 1);
		compute_primes_local(0, 25000);
		gpio_put(expe_pin, 0);
		sleep_ms(100);
		
		gpio_put(expe_pin, 1);
		timer_busy();
		gpio_put(expe_pin, 0);
		sleep_ms(100);
		
		gpio_put(expe_pin, 1);
		sleep_ms(10000);
		gpio_put(expe_pin, 0);
		sleep_ms(100);
	}
	//gpio_init(expe_pin);
	//gpio_set_dir(expe_pin, GPIO_OUT);
	//gpio_put(expe_pin, 1);
	//while(true) {
	//	//sleep_ms(10000);
	//}
	
	//absolute_time_t t = make_timeout_time_us(1000000 - PICO_TIME_SLEEP_OVERHEAD_ADJUST_US);
	//sync_internal_yield_until_before(t);
	//busy_wait_until(t);
	//printf("main()\n");
	//__sev();
	//__wfe();
	//
	//sleep_ms(3000);

	////pll_init(pll_sys, PLL_SYS_REFDIV, PLL_SYS_VCO_FREQ_HZ, PLL_SYS_POSTDIV1, PLL_SYS_POSTDIV2);
	////pll_init(pll_usb, PLL_USB_REFDIV, PLL_USB_VCO_FREQ_HZ, PLL_USB_POSTDIV1, PLL_USB_POSTDIV2);
	////xosc_init();
	////rosc_enable();
	//while(true) {
	//	absolute_time_t t = make_timeout_time_us(1000000 - PICO_TIME_SLEEP_OVERHEAD_ADJUST_US);
	//	sync_internal_yield_until_before(t);
	//	busy_wait_until(t);
	//	printf("in while loop\n");
	//}
}

