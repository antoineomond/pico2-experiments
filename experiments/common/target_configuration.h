#ifndef TARGET_CONFIGURATION_H
#define TARGET_CONFIGURATION_H

#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/pll.h"
#include "hardware/vreg.h"
#include "hardware/powman.h"
#include "hardware/pll.h"
#include "hardware/xosc.h"
#include "hardware/ticks.h"
#include "hardware/watchdog.h"
#include "pico/sleep.h"
#include "hardware/clocks.h"

#define PLL_MIN_VCO_FREQ_HZ 760*MHZ 
#define PLL_MAX_POSTDIV 7 
#define PLL_DEFAULT_VCO_FREQ_HZ PLL_SYS_VCO_FREQ_HZ 
#define PLL_DEFAULT_POSTDIV1 PLL_SYS_POSTDIV1
#define PLL_DEFAULT_POSTDIV2 PLL_SYS_POSTDIV2

#define ROSC_MAX_DIVIDER 30
#define ROSC_MIN_RANGE ROSC_CTRL_FREQ_RANGE_VALUE_LOW
#define ROSC_MIN_DRIVE_STRENGTH 0x0000
#define ROSC_MAX_DRIVE_STRENGTH 0x7777
#define ROSC_DEFAULT_DIVIDER 8
#define ROSC_DEFAULT_RANGE ROSC_MIN_RANGE
#define ROSC_DEFAULT_DRIVE_STRENGTH ROSC_MIN_DRIVE_STRENGTH

#define LPOSC_MIN_TRIM 0x000
#define LPOSC_MAX_TRIM 0x3f0
#define LPOSC_DEFAULT_TRIM LPOSC_MIN_TRIM

#define VREG_DEFAULT VREG_VOLTAGE_DEFAULT
#define VREG_MIN_PLL VREG_VOLTAGE_0_90
#define VREG_MIN_XOSC_ROSC_LPOSC VREG_VOLTAGE_0_75

enum CLOCK_SOURCE {
	PLL_SYS, XOSC, ROSC, LPOSC
};

struct config {
	// Clock source
	uint clock_source;
	
	// Clock frequency
	// PLL
  uint pll_vco_freq;
  uint pll_div1;
  uint pll_div2;
	
	// ROSC
  uint rosc_div;
  uint rosc_range;
  uint rosc_drive_freqa;
  uint rosc_drive_freqb;
	
	// LPOSC
	uint lposc_trim;
	
	// VREG output
	uint vreg_output;
	
	// Set clock source as reference clock
	bool set_as_ref;
};
extern float TIME_RATE;

static struct config configs[] = {
	/* pll min f v */
	//{PLL_SYS, PLL_MIN_VCO_FREQ_HZ, PLL_MAX_POSTDIV, PLL_MAX_POSTDIV, 0, 0, 0, 0, 0, VREG_VOLTAGE_0_90, true},																						 // 22.62 mW
	//{PLL_SYS, PLL_MIN_VCO_FREQ_HZ, PLL_MAX_POSTDIV, PLL_MAX_POSTDIV, 0, 0, 0, 0, 0, VREG_VOLTAGE_0_95, true},																						 // 22.62 mW
	//{PLL_SYS, PLL_MIN_VCO_FREQ_HZ, PLL_MAX_POSTDIV, PLL_MAX_POSTDIV, 0, 0, 0, 0, 0, VREG_VOLTAGE_1_00, true},																				 // 22.62 mW
	//{PLL_SYS, PLL_MIN_VCO_FREQ_HZ, PLL_MAX_POSTDIV, PLL_MAX_POSTDIV, 0, 0, 0, 0, 0, VREG_VOLTAGE_1_05, true},																				 // 22.62 mW
	//{PLL_SYS, PLL_MIN_VCO_FREQ_HZ, PLL_MAX_POSTDIV, PLL_MAX_POSTDIV, 0, 0, 0, 0, 0, VREG_VOLTAGE_1_10, true},																				 // 22.62 mW

	/* Bench compute time per clock freq */
	//{PLL_SYS, 1500 * MHZ, 5, 2, 0, 0, 0, 0, 0, VREG_DEFAULT, true},															 // 73.99 mW
	//{PLL_SYS, 1500 * MHZ, 6, 2, 0, 0, 0, 0, 0, VREG_DEFAULT, true},
	//{PLL_SYS, 1500 * MHZ, 5, 3, 0, 0, 0, 0, 0, VREG_DEFAULT, true},
	//{PLL_SYS, 1500 * MHZ, 5, 4, 0, 0, 0, 0, 0, VREG_DEFAULT, true},
	//{PLL_SYS, 1500 * MHZ, 5, 6, 0, 0, 0, 0, 0, VREG_DEFAULT, true},

	/* Alternate 1.1V PLL to 0.8V LPOSC */
	//{PLL_SYS, PLL_DEFAULT_VCO_FREQ_HZ, PLL_DEFAULT_POSTDIV1, PLL_DEFAULT_POSTDIV2, 0, 0, 0, 0, 0, VREG_DEFAULT, true},															 // 73.99 mW
	//{LPOSC, 0, 0, 0, 0, 0, 0, 0, 0x00, VREG_VOLTAGE_0_80, true}, // 3.49 mW
	
	// ROSC range lower 0.9V
	//{ROSC, 0, 0, 0, 5, 0xfa7, 0x0017, 0x0077, 0, VREG_VOLTAGE_0_90, true},  // 20.001MHz
	//{ROSC, 0, 0, 0, 3, 0xfa7, 0x0003, 0x0017, 0, VREG_VOLTAGE_0_90, true},  // 30.004MHz
	//{ROSC, 0, 0, 0, 2, 0xfa5, 0x0077, 0x0003, 0, VREG_VOLTAGE_0_90, true},  // 40.007MHz
	//{ROSC, 0, 0, 0, 2, 0xfa7, 0x0017, 0x0577, 0, VREG_VOLTAGE_0_90, true},  // 50.020MHz
	//{ROSC, 0, 0, 0, 1, 0xfa4, 0x0057, 0x0005, 0, VREG_VOLTAGE_0_90, true},  // 60.071MHz
	//{ROSC, 0, 0, 0, 1, 0xfa5, 0x0003, 0x0001, 0, VREG_VOLTAGE_0_90, true},  // 70.042MHz
	//{ROSC, 0, 0, 0, 1, 0xfa4, 0x5777, 0x0177, 0, VREG_VOLTAGE_0_90, true},  // 80.081MHz
	//{ROSC, 0, 0, 0, 1, 0xfa7, 0x0003, 0x0037, 0, VREG_VOLTAGE_0_90, true},  // 90.040MHz
	//{ROSC, 0, 0, 0, 1, 0xfa7, 0x0017, 0x0777, 0, VREG_VOLTAGE_0_90, true},  // 100.043MHz
	//{ROSC, 0, 0, 0, 1, 0xfa5, 0x7777, 0x0077, 0, VREG_VOLTAGE_0_90, true},  // 109.679MHz
	//{ROSC, 0, 0, 0, 1, 0xfa7, 0x1777, 0x0077, 0, VREG_VOLTAGE_0_90, true},  // 130.003MHz
	
	/* Paper experiments */
	// ROSC range lower 1.1V
	//{ROSC, 0, 0, 0, 20, 0xfa7, 0x0177, 0x0077, 0, VREG_DEFAULT, true}, // 10MHz
	//{ROSC, 0, 0, 0, 5, 0xfa4, 0x0017, 0x0001, 0, VREG_DEFAULT, true}, // 20.029MHz
	//{ROSC, 0, 0, 0, 4, 0xfa4, 0x3777, 0x0005, 0, VREG_DEFAULT, true},  // 30.071MHz
	//{ROSC, 0, 0, 0, 4, 0xfa4, 0x3777, 0x5777, 0, VREG_DEFAULT, true},  // 40.002MHz
	//{ROSC, 0, 0, 0, 4, 0xfa7, 0x0377, 0x0077, 0, VREG_DEFAULT, true},  // 50.000MHz
	//{ROSC, 0, 0, 0, 3, 0xfa7, 0x0057, 0x0001, 0, VREG_DEFAULT, true},  // 59.984MHz
	//{ROSC, 0, 0, 0, 2, 0xfa4, 0x0577, 0x1777, 0, VREG_DEFAULT, true},  // 69.931MHz
	//{ROSC, 0, 0, 0, 2, 0xfa5, 0x0377, 0x0017, 0, VREG_DEFAULT, true},  // 80.055MHz
	//{ROSC, 0, 0, 0, 2, 0xfa7, 0x0057, 0x0001, 0, VREG_DEFAULT, true},  // 90.004MHz
	//{ROSC, 0, 0, 0, 2, 0xfa7, 0x0377, 0x0077, 0, VREG_DEFAULT, true},  // 100.040MHz
	//{ROSC, 0, 0, 0, 1, 0xfa4, 0x0017, 0x0077, 0, VREG_DEFAULT, true},  // 110.003MHz
	//{ROSC, 0, 0, 0, 1, 0xfa4, 0x5777, 0x0003, 0, VREG_DEFAULT, true},  // 120.244MHz
	//{ROSC, 0, 0, 0, 1, 0xfa5, 0x0003, 0x0017, 0, VREG_DEFAULT, true},  // 130.148MHz
	//{ROSC, 0, 0, 0, 1, 0xfa4, 0x0177, 0x7777, 0, VREG_DEFAULT, true},  // 140.367MHz
	//{ROSC, 0, 0, 0, 1, 0xfa5, 0x0057, 0x0037, 0, VREG_DEFAULT, true},  // 149.699MHz
	
	
	// ROSC range lower low V 
	//{ROSC, 0, 0, 0, 4, 0xfa5, 0x0577, 0x0037, 0, VREG_VOLTAGE_0_85, true},  // 20.015MHz
	//{ROSC, 0, 0, 0, 2, 0xfa5, 0x0003, 0x0003, 0, VREG_VOLTAGE_0_85, true},  // 30.014MHz
	//{ROSC, 0, 0, 0, 2, 0xfa5, 0x0577, 0x0037, 0, VREG_VOLTAGE_0_85, true},  // 40.025MHz
	//{ROSC, 0, 0, 0, 1, 0xfa4, 0x0007, 0x0077, 0, VREG_VOLTAGE_0_85, true},  // 50.031MHz
	//{ROSC, 0, 0, 0, 1, 0xfa5, 0x0003, 0x0003, 0, VREG_VOLTAGE_0_85, true},  // 59.945MHz
	//{ROSC, 0, 0, 0, 1, 0xfa4, 0x0777, 0x7777, 0, VREG_VOLTAGE_0_85, true},  // 70.069MHz
	//{ROSC, 0, 0, 0, 1, 0xfa5, 0x0577, 0x0037, 0, VREG_VOLTAGE_0_85, true},  // 80.110MHz
	//{ROSC, 0, 0, 0, 1, 0xfa5, 0x3777, 0x0057, 0, VREG_VOLTAGE_0_85, true},  // 89.910MHz
	//{ROSC, 0, 0, 0, 1, 0xfa7, 0x0777, 0x0001, 0, VREG_VOLTAGE_0_85, true},  // 99.675MHz
	//{ROSC, 0, 0, 0, 1, 0xfa7, 0x1777, 0x0001, 0, VREG_VOLTAGE_0_85, true},  // 109.626MHz
	//{ROSC, 0, 0, 0, 1, 0xfa7, 0x0017, 0x0007, 0, VREG_VOLTAGE_0_95, true},  // 120.673MHz
	//{ROSC, 0, 0, 0, 1, 0xfa5, 0x5777, 0x0777, 0, VREG_VOLTAGE_0_95, true},  // 130.454MHz
	//{ROSC, 0, 0, 0, 1, 0xfa7, 0x0377, 0x0377, 0, VREG_VOLTAGE_0_95, true},  // 140.061MHz
	//{ROSC, 0, 0, 0, 1, 0xfa5, 0x7777, 0x3777, 0, VREG_VOLTAGE_1_00, true},  // 150.328MHz
	//{ROSC, 0, 0, 0, 1, 0xfa7, 0x1777, 0x0001, 0, VREG_VOLTAGE_0_95, true},  // 158.643MHz
	//{ROSC, 0, 0, 0, 1, 0xfa7, 0x7777, 0x0001, 0, VREG_VOLTAGE_0_95, true},  // 167.514MHz
	//{ROSC, 0, 0, 0, 1, 0xfa7, 0x1777, 0x0007, 0, VREG_VOLTAGE_1_00, true},  // 181.769MHz
	//{ROSC, 0, 0, 0, 1, 0xfa7, 0x3777, 0x0001, 0, VREG_VOLTAGE_1_00, true},  // 189.855MHz
	//{ROSC, 0, 0, 0, 1, 0xfa7, 0x1777, 0x0007, 0, VREG_VOLTAGE_1_05, true},  // 205.385MHz
	//{ROSC, 0, 0, 0, 1, 0xfa7, 0x1777, 0x0001, 0, VREG_VOLTAGE_1_05, true},  // 206.788MHz
	//{ROSC, 0, 0, 0, 1, 0xfa7, 0x7777, 0x0001, 0, VREG_VOLTAGE_1_05, true},  // 218.938MHz
	//{ROSC, 0, 0, 0, 1, 0xfa7, 0x1777, 0x0007, 0, VREG_VOLTAGE_1_10, true},  // 231.263MHz
	//{ROSC, 0, 0, 0, 1, 0xfa7, 0x5777, 0x0017, 0, VREG_VOLTAGE_1_10, true},  // 240.461MHz
	//{ROSC, 0, 0, 0, 1, 0xfa7, 0x7777, 0x0001, 0, VREG_VOLTAGE_1_10, true},  // 246.805MHz

	// PLL range
	// The maximum frequency the board can reach at 1.1V is 312MHz (https://learn.pimoroni.com/article/overclocking-the-pico-2)
	//{PLL_SYS, 840*MHZ, 7, 6, 0, 0, 0, 0, 0, VREG_DEFAULT, true},  // 20MHz 
	//{PLL_SYS, 1260*MHZ, 7, 6, 0, 0, 0, 0, 0, VREG_DEFAULT, true}, // 30MHz 
	//{PLL_SYS, 1440*MHZ, 6, 6, 0, 0, 0, 0, 0, VREG_DEFAULT, true}, // 40MHz 
	//{PLL_SYS, 1500*MHZ, 6, 5, 0, 0, 0, 0, 0, VREG_DEFAULT, true}, // 50MHz 
	//{PLL_SYS, 1500*MHZ, 5, 5, 0, 0, 0, 0, 0, VREG_DEFAULT, true}, // 60MHz 
	//{PLL_SYS, 1260*MHZ, 6, 3, 0, 0, 0, 0, 0, VREG_DEFAULT, true}, // 70MHz 
	//{PLL_SYS, 1440*MHZ, 6, 3, 0, 0, 0, 0, 0, VREG_DEFAULT, true}, // 80MHz 
	//{PLL_SYS, 1440*MHZ, 4, 4, 0, 0, 0, 0, 0, VREG_DEFAULT, true}, // 90MHz 
	//{PLL_SYS, 1500*MHZ, 5, 3, 0, 0, 0, 0, 0, VREG_DEFAULT, true}, // 100MHz 
	//{PLL_SYS, 1320*MHZ, 6, 2, 0, 0, 0, 0, 0, VREG_DEFAULT, true}, // 110MHz 
	//{PLL_SYS, 1440*MHZ, 6, 2, 0, 0, 0, 0, 0, VREG_DEFAULT, true}, // 120MHz 
	//{PLL_SYS, 1560*MHZ, 6, 2, 0, 0, 0, 0, 0, VREG_DEFAULT, true}, // 130MHz 
	//{PLL_SYS, 1260*MHZ, 3, 3, 0, 0, 0, 0, 0, VREG_DEFAULT, true}, // 140MHz 
	//{PLL_SYS, 1500*MHZ, 2, 5, 0, 0, 0, 0, 0, VREG_DEFAULT, true}, // 150MHz 
	//{PLL_SYS, 1440*MHZ, 3, 3, 0, 0, 0, 0, 0, VREG_DEFAULT, true}, // 160MHz 
	//{PLL_SYS, 1360*MHZ, 2, 4, 0, 0, 0, 0, 0, VREG_DEFAULT, true}, // 170MHz 
	//{PLL_SYS, 1440*MHZ, 2, 4, 0, 0, 0, 0, 0, VREG_DEFAULT, true}, // 180MHz 
	//{PLL_SYS, 1140*MHZ, 1, 6, 0, 0, 0, 0, 0, VREG_DEFAULT, true}, // 190MHz 
	//{PLL_SYS, 1200*MHZ, 1, 6, 0, 0, 0, 0, 0, VREG_DEFAULT, true}, // 200MHz 
	//{PLL_SYS, 1260*MHZ, 1, 6, 0, 0, 0, 0, 0, VREG_DEFAULT, true}, // 210MHz 
	//{PLL_SYS, 1320*MHZ, 1, 6, 0, 0, 0, 0, 0, VREG_DEFAULT, true}, // 220MHz 
	//{PLL_SYS, 1380*MHZ, 1, 6, 0, 0, 0, 0, 0, VREG_DEFAULT, true}, // 230MHz 
	//{PLL_SYS, 1440*MHZ, 1, 6, 0, 0, 0, 0, 0, VREG_DEFAULT, true}, // 240MHz 
	//{PLL_SYS, 1500*MHZ, 1, 6, 0, 0, 0, 0, 0, VREG_DEFAULT, true}, // 250MHz 
																																// 
	// PLL range low vreg
	// The maximum frequency the board can reach at 1.1V is 312MHz (https://learn.pimoroni.com/article/overclocking-the-pico-2)
	//{PLL_SYS, 840*MHZ, 7, 6, 0, 0, 0, 0, 0,  VREG_VOLTAGE_0_90, true},  // 20MHz 
	//{PLL_SYS, 1260*MHZ, 7, 6, 0, 0, 0, 0, 0, VREG_VOLTAGE_0_90, true}, // 30MHz 
	//{PLL_SYS, 1440*MHZ, 6, 6, 0, 0, 0, 0, 0, VREG_VOLTAGE_0_90, true}, // 40MHz 
	//{PLL_SYS, 1500*MHZ, 6, 5, 0, 0, 0, 0, 0, VREG_VOLTAGE_0_90, true}, // 50MHz 
	//{PLL_SYS, 1500*MHZ, 5, 5, 0, 0, 0, 0, 0, VREG_VOLTAGE_0_90, true}, // 60MHz 
	//{PLL_SYS, 1260*MHZ, 6, 3, 0, 0, 0, 0, 0, VREG_VOLTAGE_0_90, true}, // 70MHz 
	//{PLL_SYS, 1440*MHZ, 6, 3, 0, 0, 0, 0, 0, VREG_VOLTAGE_0_90, true}, // 80MHz 
	//{PLL_SYS, 1440*MHZ, 4, 4, 0, 0, 0, 0, 0, VREG_VOLTAGE_0_90, true}, // 90MHz 
	//{PLL_SYS, 1500*MHZ, 5, 3, 0, 0, 0, 0, 0, VREG_VOLTAGE_0_90, true}, // 100MHz 
	//{PLL_SYS, 1320*MHZ, 6, 2, 0, 0, 0, 0, 0, VREG_VOLTAGE_0_90, true}, // 110MHz 
	//{PLL_SYS, 1440*MHZ, 6, 2, 0, 0, 0, 0, 0, VREG_VOLTAGE_0_90, true}, // 120MHz 
	//{PLL_SYS, 1560*MHZ, 6, 2, 0, 0, 0, 0, 0, VREG_VOLTAGE_0_90, true}, // 130MHz 
	//{PLL_SYS, 1260*MHZ, 3, 3, 0, 0, 0, 0, 0, VREG_VOLTAGE_0_90, true}, // 140MHz 
	//{PLL_SYS, 1500*MHZ, 2, 5, 0, 0, 0, 0, 0, VREG_VOLTAGE_0_90, true}, // 150MHz 
	//{PLL_SYS, 1440*MHZ, 3, 3, 0, 0, 0, 0, 0, VREG_VOLTAGE_0_90, true}, // 160MHz 
	//{PLL_SYS, 1360*MHZ, 2, 4, 0, 0, 0, 0, 0, VREG_VOLTAGE_0_90, true}, // 170MHz 
	//{PLL_SYS, 1440*MHZ, 2, 4, 0, 0, 0, 0, 0, VREG_VOLTAGE_0_90, true}, // 180MHz 
	//{PLL_SYS, 1140*MHZ, 1, 6, 0, 0, 0, 0, 0, VREG_VOLTAGE_0_95, true}, // 190MHz 
	//{PLL_SYS, 1200*MHZ, 1, 6, 0, 0, 0, 0, 0, VREG_VOLTAGE_0_95, true}, // 200MHz 
	//{PLL_SYS, 1260*MHZ, 1, 6, 0, 0, 0, 0, 0, VREG_VOLTAGE_0_95, true}, // 210MHz 
	//{PLL_SYS, 1320*MHZ, 1, 6, 0, 0, 0, 0, 0, VREG_VOLTAGE_0_95, true}, // 220MHz 
	//{PLL_SYS, 1380*MHZ, 1, 6, 0, 0, 0, 0, 0, VREG_VOLTAGE_1_00, true}, // 230MHz 
	//{PLL_SYS, 1440*MHZ, 1, 6, 0, 0, 0, 0, 0, VREG_VOLTAGE_1_00, true}, // 240MHz 
	//{PLL_SYS, 1500*MHZ, 1, 6, 0, 0, 0, 0, 0, VREG_VOLTAGE_1_00, true}, // 250MHz 
	
	// VCO freqs
	//{PLL_SYS, PLL_DEFAULT_VCO_FREQ_HZ, 2, 5, 0, 0, 0, 0, 0, VREG_DEFAULT, true}, // 150MHz 
	//{PLL_SYS, 900*MHZ, 2, 3, 0, 0, 0, 0, 0, VREG_DEFAULT, true},								 // 150MHz
	//{PLL_SYS, 1480*MHZ, 7, 7, 0, 0, 0, 0, 0, VREG_DEFAULT, true},								 // 30.123MHz
	//{PLL_SYS, 760*MHZ, 5, 5, 0, 0, 0, 0, 0, VREG_DEFAULT, true},								 // 30.240MHz
	
	// Baseline
	{PLL_SYS, PLL_DEFAULT_VCO_FREQ_HZ, PLL_DEFAULT_POSTDIV1, PLL_DEFAULT_POSTDIV2, 0, 0, 0, 0, 0, VREG_DEFAULT, true},															 // 73.99 mW
	{XOSC, 0, 0, 0, 0, 0, 0, 0, 0, VREG_DEFAULT, true},																																															 // 14.85 mW
	{ROSC, 0, 0, 0, ROSC_DEFAULT_DIVIDER, ROSC_DEFAULT_RANGE, ROSC_DEFAULT_DRIVE_STRENGTH, ROSC_DEFAULT_DRIVE_STRENGTH, 0, VREG_DEFAULT, true},			 // 13.14 mW
	
	// pll Min frequency, default voltage
	{PLL_SYS, PLL_MIN_VCO_FREQ_HZ, PLL_MAX_POSTDIV, PLL_MAX_POSTDIV, 0, 0, 0, 0, 0, VREG_DEFAULT, true},																						 // 18.10 mW
	// pll Default frequency, min voltage
	{PLL_SYS, PLL_DEFAULT_VCO_FREQ_HZ, PLL_DEFAULT_POSTDIV1, PLL_DEFAULT_POSTDIV2, 0, 0, 0, 0, 0, VREG_MIN_PLL, true},															 // 54.18 mW
	// pll Min frequency, min voltage
	{PLL_SYS, PLL_MIN_VCO_FREQ_HZ, PLL_MAX_POSTDIV, PLL_MAX_POSTDIV, 0, 0, 0, 0, 0, VREG_MIN_PLL, true},																						 // 22.62 mW
																																																																								  
	// rosc Min frequency, default voltage
	{ROSC, 0, 0, 0, ROSC_MAX_DIVIDER, ROSC_MIN_RANGE, ROSC_MIN_DRIVE_STRENGTH, ROSC_MIN_DRIVE_STRENGTH, 0, VREG_DEFAULT, true},											 // 9.82 mW
	// rosc Default frequency, min voltage
	{ROSC, 0, 0, 0, ROSC_DEFAULT_DIVIDER, ROSC_DEFAULT_RANGE, ROSC_DEFAULT_DRIVE_STRENGTH, ROSC_DEFAULT_DRIVE_STRENGTH, 0, VREG_VOLTAGE_0_80, true}, // 5.94 mW
	// rosc Min frequency, min voltage
	{ROSC, 0, 0, 0, ROSC_MAX_DIVIDER, ROSC_MIN_RANGE, ROSC_MIN_DRIVE_STRENGTH, ROSC_MIN_DRIVE_STRENGTH, 0, VREG_VOLTAGE_0_80, true},					       // 4.24 mW
																																																																							 
	// Min frequency, min voltage
	{XOSC, 0, 0, 0, 0, 0, 0, 0, 0, VREG_VOLTAGE_1_00, true},
	{XOSC, 0, 0, 0, 0, 0, 0, 0, 0, VREG_VOLTAGE_0_90, true},
	{XOSC, 0, 0, 0, 0, 0, 0, 0, 0, VREG_VOLTAGE_0_80, true},
	
	// lposc Default trim
	{LPOSC, 0, 0, 0, 0, 0, 0, 0, 0x20, VREG_VOLTAGE_1_10, true}, // 5.48 mW
	{LPOSC, 0, 0, 0, 0, 0, 0, 0, 0x20, VREG_VOLTAGE_1_00, true}, // 4.53 mW
	{LPOSC, 0, 0, 0, 0, 0, 0, 0, 0x20, VREG_VOLTAGE_0_90, true}, // 3.89 mW
	{LPOSC, 0, 0, 0, 0, 0, 0, 0, 0x20, VREG_VOLTAGE_0_80, true}, // 3.48 mW

	// lposc Max frequency (trim)                                        
	{LPOSC, 0, 0, 0, 0, 0, 0, 0, 0xf0, VREG_VOLTAGE_1_10, true}, // 5.43 mW
	{LPOSC, 0, 0, 0, 0, 0, 0, 0, 0xf0, VREG_VOLTAGE_1_00, true}, // 4.53 mW
	{LPOSC, 0, 0, 0, 0, 0, 0, 0, 0xf0, VREG_VOLTAGE_0_90, true}, // 3.90 mW
	{LPOSC, 0, 0, 0, 0, 0, 0, 0, 0xf0, VREG_VOLTAGE_0_80, true}, // 3.49 mW

	// lposc Min frequency (no trim), min voltage                        
	{LPOSC, 0, 0, 0, 0, 0, 0, 0, 0x00, VREG_VOLTAGE_1_10, true}, // 5.43 mW
	{LPOSC, 0, 0, 0, 0, 0, 0, 0, 0x00, VREG_VOLTAGE_1_00, true}, // 4.53 mW
	{LPOSC, 0, 0, 0, 0, 0, 0, 0, 0x00, VREG_VOLTAGE_0_90, true}, // 3.90 mW
	{LPOSC, 0, 0, 0, 0, 0, 0, 0, 0x00, VREG_VOLTAGE_0_80, true}, // 3.49 mW
};
static const uint nb_expes = sizeof(configs)/sizeof(configs[0]);

static inline void start_all_ticks(void) {
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

static inline void restart_all_ticks(void) {
	for (int i = 0; i < (int)TICK_COUNT; ++i) {
			tick_stop((tick_gen_num_t)i);
			while(tick_is_running((tick_gen_num_t)i)) tight_loop_contents();
	}
	start_all_ticks();
}

static inline uint set_clock_source_xosc() {
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

// Clock source leverages
static inline uint set_clock_source_lposc(uint trim) {
	// lposc has to be clk_ref
	// Put xosc as clk_ref to count lposc frequency
	//xosc_init();
	//clock_configure_undivided(clk_ref, CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC, 0, XOSC_HZ);
	//restart_all_ticks();
	set_clock_source_xosc();
	
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
static inline uint set_clock_source_rosc(uint div, uint range, uint freqa, uint freqb) {
	rosc_enable();
	
	// Put xosc as clk_ref to count rosc frequency
	xosc_init();
	clock_configure_undivided(clk_ref, CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC, 0, XOSC_HZ);
	//restart_all_ticks();
	
	// Specify rosc frequency
	rosc_set_div(div);
	rosc_set_range(range);
	rosc_write(&rosc_hw->freqa, (ROSC_FREQA_PASSWD_VALUE_PASS << ROSC_FREQA_PASSWD_LSB) | freqa);
	rosc_write(&rosc_hw->freqb, (ROSC_FREQA_PASSWD_VALUE_PASS << ROSC_FREQA_PASSWD_LSB) | freqb);
	
	uint clk_src_freq = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_ROSC_CLKSRC) * KHZ;
	
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

static inline uint set_clock_source_pll(uint vco_freq, uint div1, uint div2) {
	set_sys_clock_pll(vco_freq, div1, div2);
	sleep_ms(100);
	
	uint clk_src_freq = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_PLL_SYS_CLKSRC_PRIMARY) * KHZ;
	
	clock_configure_undivided(clk_sys, CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX, CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS, clk_src_freq);
	clock_configure_undivided(clk_peri, 0, CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS, clk_src_freq);
	
	// Disable unused clock sources
	pll_deinit(pll_usb);
	rosc_disable();
	
	return clk_src_freq;
}

static inline uint switch_configuration_from_parameter(const struct config* config) {
	// Set the voltage, clock source and frequency (measure the frequency for rosc and lposc)
	// pll must be deactivated to reach vreg outputs below 0.9V
	if(config->vreg_output <= VREG_VOLTAGE_0_85)
		set_clock_source_xosc();
	sleep_ms(100);
	vreg_disable_voltage_limit();
	powman_clear_bits(&powman_hw->bod, 0x000001f1);
	vreg_set_voltage(config->vreg_output);
	sleep_ms(100);
	
	uint clk_src_freq;
	if(config->clock_source == PLL_SYS) {
		clk_src_freq = set_clock_source_pll(config->pll_vco_freq, config->pll_div1, config->pll_div2);
	}
	if(config->clock_source == XOSC) {
		clk_src_freq = set_clock_source_xosc();
	}
	if(config->clock_source == ROSC) {
		clk_src_freq = set_clock_source_rosc(config->rosc_div, config->rosc_range, config->rosc_drive_freqa, config->rosc_drive_freqb);
	}
	if(config->clock_source == LPOSC) {
		clk_src_freq = set_clock_source_lposc(config->lposc_trim);
	}
	return clk_src_freq;
}

static inline void switch_to_default_configuration() {
	xosc_init();
	clock_configure_undivided(clk_ref, CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC, 0, XOSC_HZ);
	clock_configure_undivided(clk_sys, CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX, CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_XOSC_CLKSRC, XOSC_HZ);
	restart_all_ticks();
	hw_clear_bits(&powman_hw->vreg_ctrl, POWMAN_PASSWORD_BITS | POWMAN_VREG_CTRL_DISABLE_VOLTAGE_LIMIT_BITS);
	powman_clear_bits(&powman_hw->bod, 0x000001f1);
	powman_set_bits(&powman_hw->bod, POWMAN_BOD_VSEL_RESET);
	vreg_set_voltage(VREG_VOLTAGE_DEFAULT);
	sleep_us((int)(10*1000000*TIME_RATE));
	pll_deinit(pll_sys);
	pll_deinit(pll_usb);
	pll_init(pll_sys, PLL_SYS_REFDIV, PLL_SYS_VCO_FREQ_HZ, PLL_SYS_POSTDIV1, PLL_SYS_POSTDIV2);
	pll_init(pll_usb, PLL_USB_REFDIV, PLL_USB_VCO_FREQ_HZ, PLL_USB_POSTDIV1, PLL_USB_POSTDIV2);
	clock_configure_undivided(clk_sys, CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX, CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS, SYS_CLK_HZ);
	clock_configure_undivided(clk_peri,
									0,
									CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS,
									SYS_CLK_HZ);
	clock_configure_undivided(clk_usb,
									0, // No GLMUX
									CLOCKS_CLK_USB_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB,
									USB_CLK_HZ);
	clock_configure_undivided(clk_adc,
									0, // No GLMUX
									CLOCKS_CLK_ADC_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB,
									USB_CLK_HZ);
	clock_configure_undivided(clk_hstx,
									0,
									CLOCKS_CLK_HSTX_CTRL_AUXSRC_VALUE_CLK_SYS,
									SYS_CLK_HZ);
	
	stdio_init_all();
	sleep_ms(1000);
}

static inline void print_configuration(const struct config* config) {
	if(config->clock_source == PLL_SYS) {
		printf("clock source: PLL\n");
		printf("frequency vco: %dMHz\n", config->pll_vco_freq/1000000);
		printf("divider 1: %d\n", config->pll_div1);
		printf("divider 2: %d\n", config->pll_div2);
	}
	if(config->clock_source == XOSC) {
		printf("clock source: XOSC\n");
	}
	if(config->clock_source == ROSC) {
		printf("clock source: ROSC\n");
		printf("rosc divider: %d\n", config->rosc_div);
		printf("rosc range: 0x%x\n", config->rosc_range);
		printf("drive strength a: 0x%x\n", config->rosc_drive_freqa);
		printf("drive strength b: 0x%x\n", config->rosc_drive_freqb);
	}
	if(config->clock_source == LPOSC) {
		printf("clock source: LPOSC\n");
		printf("trim register value: 0x%x\n", config->lposc_trim);
	}
	const char* vreg_strings[] = {"0.55V", "0.60V", "0.65V", "0.70V", "0.75V", "0.80V", "0.85V", "0.90V", "0.95V", "1.00V", "1.05V", "1.10V"};
	printf("VREG output: %s\n", vreg_strings[config->vreg_output]);
	printf("is reference clock: %d\n", config->set_as_ref);
}
#endif
