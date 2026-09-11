#include <pico/stdio.h>
#include <stdint.h>
#include <stdio.h>
#include "experiments.h"
#include "target_configuration.h"
#include "hardware/structs/usb.h"
#include "pico/time.h"
#include "pico/sync.h"
#include "hardware/timer.h"

extern int expe_pin;
//void pull_down_gpios() {
//	const uint used_gpios[] = {expe_pin};
//	size_t nb_gpios = sizeof(used_gpios)/sizeof(used_gpios[0]);
//	for (int gpio = 0; gpio < NUM_BANK0_GPIOS; gpio++) {
//		if(!expe_pin) {
//			gpio_set_dir(gpio, 0);
//			gpio_set_function(gpio, GPIO_FUNC_SIO);
//			if(gpio > NUM_BANK0_GPIOS - NUM_ADC_CHANNELS) {
//				gpio_disable_pulls(gpio);
//				gpio_set_input_enabled(gpio, false);
//			}
//		}
//	}
//}
//
//void turn_off_clocks() {
//	clock_hw_t *clock_hw = &clocks_hw->clk[clk_usb];
//	hw_clear_bits(&clock_hw->ctrl, CLOCKS_CLK_USB_CTRL_ENABLE_BITS);
//	clock_hw = &clocks_hw->clk[clk_adc];
//	hw_clear_bits(&clock_hw->ctrl, CLOCKS_CLK_ADC_CTRL_ENABLE_BITS);
//	clock_hw = &clocks_hw->clk[clk_hstx];
//	hw_clear_bits(&clock_hw->ctrl, CLOCKS_CLK_HSTX_CTRL_ENABLE_BITS);
//}
//
//static void disable_usb() {
//    usb_hw->phy_direct = USB_USBPHY_DIRECT_TX_PD_BITS | USB_USBPHY_DIRECT_RX_PD_BITS | USB_USBPHY_DIRECT_DM_PULLDN_EN_BITS | USB_USBPHY_DIRECT_DP_PULLDN_EN_BITS;
//    
//    usb_hw->phy_direct_override = USB_USBPHY_DIRECT_RX_DM_BITS | USB_USBPHY_DIRECT_RX_DP_BITS |          USB_USBPHY_DIRECT_RX_DD_BITS |
//        USB_USBPHY_DIRECT_OVERRIDE_TX_DIFFMODE_OVERRIDE_EN_BITS | USB_USBPHY_DIRECT_OVERRIDE_DM_PULLUP_OVERRIDE_EN_BITS | USB_USBPHY_DIRECT_OVERRIDE_TX_FSSLEW_OVERRIDE_EN_BITS |
//        USB_USBPHY_DIRECT_OVERRIDE_TX_PD_OVERRIDE_EN_BITS | USB_USBPHY_DIRECT_OVERRIDE_RX_PD_OVERRIDE_EN_BITS | USB_USBPHY_DIRECT_OVERRIDE_TX_DM_OVERRIDE_EN_BITS |
//        USB_USBPHY_DIRECT_OVERRIDE_TX_DP_OVERRIDE_EN_BITS | USB_USBPHY_DIRECT_OVERRIDE_TX_DM_OE_OVERRIDE_EN_BITS | USB_USBPHY_DIRECT_OVERRIDE_TX_DP_OE_OVERRIDE_EN_BITS |
//        USB_USBPHY_DIRECT_OVERRIDE_DM_PULLDN_EN_OVERRIDE_EN_BITS | USB_USBPHY_DIRECT_OVERRIDE_DP_PULLDN_EN_OVERRIDE_EN_BITS | USB_USBPHY_DIRECT_OVERRIDE_DP_PULLUP_EN_OVERRIDE_EN_BITS |
//        USB_USBPHY_DIRECT_OVERRIDE_DM_PULLUP_HISEL_OVERRIDE_EN_BITS | USB_USBPHY_DIRECT_OVERRIDE_DP_PULLUP_HISEL_OVERRIDE_EN_BITS;
//}

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


static void callback_sleep(void) {
}

void experiments_wfi() {
	// Prepare to sleep
	struct timespec ts;
	aon_timer_get_time(&ts);
	aon_timer_start(&ts); // NOTE: aon_timer_start uses xosc as 1khz clock tick by default. Change to lposc in the source code of the function so it tracks the correct amount of time
	ts.tv_sec += 3;
	aon_timer_enable_alarm(&ts, &callback_sleep, true);
	//clocks_hw->sleep_en0 = CLOCKS_SLEEP_EN0_CLK_REF_POWMAN_BITS | CLOCKS_ENABLED0_CLK_SYS_SIO_BITS | CLOCKS_ENABLED0_CLK_SYS_BUSFABRIC_BITS; // pas touche a CLK_SYS_BUSFABRIC et CLK_SYS_SIOB
	//clocks_hw->sleep_en1 = 0;
	processor_deep_sleep(); // TODO: Need to identify what deepsleep is doing on the RP2350
	//powman_set_debug_power_request_ignored(true);
	gpio_put(expe_pin, 1);
	__wfi();
	gpio_put(expe_pin, 0);
	//sleep_us((int)(200000*TIME_RATE));
	//led_blink(1);
}

void study_lposc_freq() {
	uint clk_src_freq = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_LPOSC_CLKSRC) * KHZ;
	stdio_init_all();
	sleep_ms(1000);
	printf("freq: %d\n", clk_src_freq);
	printf("freq: %d\n", clk_src_freq);
	printf("freq: %d\n", clk_src_freq);
	uint16_t* lposc_calib_data = (uint16_t*)0x40130000UL + 0x00000011;
	printf("calib freq: %d\n", powman_timer_get_lposc_calib_freq());
	printf("lposc_calib_data: %d\n", *lposc_calib_data);
}

uint32_t powman_timer_get_lposc_calib_freq(void) {
	return 30*KHZ;
}

int main() {
	//study_lposc_freq();
	//powman_timer_set_1khz_tick_source_lposc();
	//while(true){}

	gpio_init(expe_pin);
	gpio_set_dir(expe_pin, GPIO_OUT);
	static struct config config = {ROSC, 0, 0, 0, ROSC_DEFAULT_DIVIDER, ROSC_DEFAULT_RANGE, ROSC_DEFAULT_DRIVE_STRENGTH, ROSC_DEFAULT_DRIVE_STRENGTH, 0, VREG_DEFAULT, true};
	uint clk_src_freq = switch_configuration_from_parameter(&config);
	//stdio_init_all();
	//sleep_ms(1000);
	//uint16_t *clk_ref_selected = (uint16_t *)(0x40010000+0x00000038);
	//printf("CLK_REF_SELECTED ADDR: %p, ACTUAL: %p\n", clocks_hw, 0x40010000+0x000000c8));
	while(true) {
		//gpio_put(expe_pin, 1);
		//compute_primes_local(0, 25000);
		//gpio_put(expe_pin, 0);
		//sleep_ms(100);
		//
		//gpio_put(expe_pin, 1);
		//timer_busy();
		//gpio_put(expe_pin, 0);
		//sleep_ms(100);
		//
		//gpio_put(expe_pin, 1);
		//sleep_ms(10000);
		//gpio_put(expe_pin, 0);
		//sleep_ms(100);
		
		experiments_wfi();
		sleep_ms(100);
		//if(*clk_ref_selected == 0x4) {
		//	led_blink(1);
		//}
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

