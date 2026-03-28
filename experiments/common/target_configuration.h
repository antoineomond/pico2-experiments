#include "pico/stdlib.h"
#include "hardware/pll.h"
#include "hardware/vreg.h"
#include "hardware/powman.h"
#include "hardware/pll.h"
#include "hardware/xosc.h"
#include "hardware/watchdog.h"
#include "pico/sleep.h"
#include "hardware/clocks.h"

// Clock source leverages
uint leverage_clock_source_lposc(uint trim);
uint leverage_clock_source_rosc(uint div, uint range, uint freqa, uint freqb);
uint leverage_clock_source_xosc();
uint leverage_clock_source_pll(uint vco_freq, uint div1, uint div2);
void restart_all_ticks(void);

// Additional leverages (turn off unused stuff)
void pull_down_gpios();
void turn_off_clocks();
static void disable_usb();

// Sleep mode
void processor_deep_sleep(void);
