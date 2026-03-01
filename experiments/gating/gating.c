#include "hardware/clocks.h"
#include "experiments.h"

#define VALIDATION_RUN 0

// Benchmark sizes
#define BENCH_NOOP_SIZE 10
#define BENCH_PRIME_SIZE 5000
#define BENCH_MULTI_SIZE 5000
#define BENCH_MAT_SIZE 72
#define BENCH_MAT_FLOAT_SIZE 72
#define BENCH_MAT_DOUBLE_SIZE 36
#define NB_ITERATIONS_MAT_MUL 1000

void deactivate_adc() {
	clock_hw_t *clock_hw = &clocks_hw->clk[clk_adc];
	hw_clear_bits(&clock_hw->ctrl, CLOCKS_CLK_ADC_CTRL_ENABLE_BITS);
}

void deactivate_usb() {
	clock_hw_t *clock_hw = &clocks_hw->clk[clk_usb];
	hw_clear_bits(&clock_hw->ctrl, CLOCKS_CLK_USB_CTRL_ENABLE_BITS);
}

void deactivate_hstx() {
	clock_hw_t *clock_hw = &clocks_hw->clk[clk_hstx];
	hw_clear_bits(&clock_hw->ctrl, CLOCKS_CLK_HSTX_CTRL_ENABLE_BITS);
}

void deactivate_peri() {
	clock_hw_t *clock_hw = &clocks_hw->clk[clk_peri];
	hw_clear_bits(&clock_hw->ctrl, CLOCKS_CLK_PERI_CTRL_ENABLE_BITS);
}

void gating_adc() {
	clocks_hw->wake_en0 &= ~CLOCKS_WAKE_EN0_CLK_SYS_ADC_BITS &
												 ~CLOCKS_WAKE_EN0_CLK_ADC_BITS;
}

void gating_usb() {
	clocks_hw->wake_en1 &= ~CLOCKS_WAKE_EN1_CLK_SYS_USBCTRL_BITS &
												 ~CLOCKS_WAKE_EN1_CLK_USB_BITS;
}

void gating_hstx() {
	clocks_hw->wake_en0 &= ~CLOCKS_WAKE_EN0_CLK_SYS_HSTX_BITS &
												 ~CLOCKS_WAKE_EN0_CLK_HSTX_BITS;
}

void gating_peri() {
	clocks_hw->wake_en1 &= ~CLOCKS_WAKE_EN1_CLK_PERI_UART1_BITS &
												 ~CLOCKS_WAKE_EN1_CLK_PERI_UART0_BITS &
	 											 ~CLOCKS_WAKE_EN1_CLK_PERI_SPI1_BITS &
												 ~CLOCKS_WAKE_EN1_CLK_PERI_SPI0_BITS;
}

void gating_sys() {
	clocks_hw->wake_en0 &= ~CLOCKS_WAKE_EN0_CLK_SYS_SIO_BITS &
												 ~CLOCKS_WAKE_EN0_CLK_SYS_SHA256_BITS &
	 											 ~CLOCKS_WAKE_EN0_CLK_SYS_PSM_BITS &
	 											 ~CLOCKS_WAKE_EN0_CLK_SYS_ROSC_BITS &
	 											 ~CLOCKS_WAKE_EN0_CLK_SYS_ROM_BITS &
	 											 ~CLOCKS_WAKE_EN0_CLK_SYS_RESETS_BITS &
	 											 ~CLOCKS_WAKE_EN0_CLK_SYS_PWM_BITS &
	 											 ~CLOCKS_WAKE_EN0_CLK_SYS_POWMAN_BITS &
	 											 ~CLOCKS_WAKE_EN0_CLK_SYS_PLL_USB_BITS &
	 											 ~CLOCKS_WAKE_EN0_CLK_SYS_PLL_SYS_BITS &
	 											 ~CLOCKS_WAKE_EN0_CLK_SYS_PIO2_BITS &
	 											 ~CLOCKS_WAKE_EN0_CLK_SYS_PIO1_BITS &
	 											 ~CLOCKS_WAKE_EN0_CLK_SYS_PIO0_BITS &
	 											 ~CLOCKS_WAKE_EN0_CLK_SYS_PADS_BITS &
	 											 ~CLOCKS_WAKE_EN0_CLK_SYS_OTP_BITS &
	 											 ~CLOCKS_WAKE_EN0_CLK_SYS_JTAG_BITS &
	 											 ~CLOCKS_WAKE_EN0_CLK_SYS_IO_BITS &
	 											 ~CLOCKS_WAKE_EN0_CLK_SYS_I2C1_BITS &
	 											 ~CLOCKS_WAKE_EN0_CLK_SYS_I2C0_BITS &
	 											 ~CLOCKS_WAKE_EN0_CLK_SYS_HSTX_BITS &
	 											 ~CLOCKS_WAKE_EN0_CLK_SYS_GLITCH_DETECTOR_BITS &
	 											 ~CLOCKS_WAKE_EN0_CLK_SYS_DMA_BITS &
	 											 ~CLOCKS_WAKE_EN0_CLK_SYS_BUSFABRIC_BITS &
	 											 ~CLOCKS_WAKE_EN0_CLK_SYS_BUSCTRL_BITS &
	 											 ~CLOCKS_WAKE_EN0_CLK_SYS_BOOTRAM_BITS &
	 											 ~CLOCKS_WAKE_EN0_CLK_SYS_ACCESSCTRL_BITS &
	 											 ~CLOCKS_WAKE_EN0_CLK_SYS_CLOCKS_BITS &
	 											 ~CLOCKS_WAKE_EN0_CLK_SYS_BOOTRAM_BITS &
	 											 ~CLOCKS_WAKE_EN0_CLK_SYS_BOOTRAM_BITS &
	 											 ~CLOCKS_WAKE_EN0_CLK_SYS_BOOTRAM_BITS &
	 											 ~CLOCKS_WAKE_EN0_CLK_SYS_BOOTRAM_BITS &
												 ~CLOCKS_WAKE_EN0_CLK_SYS_ADC_BITS;
	clocks_hw->wake_en1 &= ~CLOCKS_WAKE_EN1_CLK_SYS_XOSC_BITS &
	 											 ~CLOCKS_WAKE_EN1_CLK_SYS_XIP_BITS &
	 											 ~CLOCKS_WAKE_EN1_CLK_SYS_WATCHDOG_BITS &
	 											 ~CLOCKS_WAKE_EN1_CLK_SYS_USBCTRL_BITS &
	 											 ~CLOCKS_WAKE_EN1_CLK_SYS_XIP_BITS &
	 											 ~CLOCKS_WAKE_EN1_CLK_SYS_UART1_BITS &
	 											 ~CLOCKS_WAKE_EN1_CLK_SYS_UART0_BITS &
	 											 ~CLOCKS_WAKE_EN1_CLK_SYS_TRNG_BITS &
	 											 ~CLOCKS_WAKE_EN1_CLK_SYS_TIMER1_BITS &
	 											 ~CLOCKS_WAKE_EN1_CLK_SYS_TIMER0_BITS &
	 											 ~CLOCKS_WAKE_EN1_CLK_SYS_TICKS_BITS &
	 											 ~CLOCKS_WAKE_EN1_CLK_SYS_TBMAN_BITS &
	 											 ~CLOCKS_WAKE_EN1_CLK_SYS_SYSINFO_BITS &
	 											 ~CLOCKS_WAKE_EN1_CLK_SYS_SYSCFG_BITS &
	 											 ~CLOCKS_WAKE_EN1_CLK_SYS_SRAM9_BITS &
	 											 ~CLOCKS_WAKE_EN1_CLK_SYS_SRAM8_BITS &
	 											 ~CLOCKS_WAKE_EN1_CLK_SYS_SRAM7_BITS &
	 											 ~CLOCKS_WAKE_EN1_CLK_SYS_SRAM6_BITS &
	 											 ~CLOCKS_WAKE_EN1_CLK_SYS_SRAM5_BITS &
	 											 ~CLOCKS_WAKE_EN1_CLK_SYS_SRAM4_BITS &
	 											 ~CLOCKS_WAKE_EN1_CLK_SYS_SRAM3_BITS &
	 											 ~CLOCKS_WAKE_EN1_CLK_SYS_SRAM2_BITS &
	 											 ~CLOCKS_WAKE_EN1_CLK_SYS_SRAM1_BITS &
	 											 ~CLOCKS_WAKE_EN1_CLK_SYS_SRAM0_BITS &
	 											 ~CLOCKS_WAKE_EN1_CLK_SYS_SPI1_BITS &
	 											 ~CLOCKS_WAKE_EN1_CLK_SYS_SPI0_BITS;

}

int main() {
	iteration_init();
	uint8_t results = execute_benchmarks(BENCH_NOOP_SIZE, BENCH_PRIME_SIZE, BENCH_MULTI_SIZE, BENCH_MAT_SIZE, BENCH_MAT_FLOAT_SIZE, BENCH_MAT_DOUBLE_SIZE, NB_ITERATIONS_MAT_MUL);
	iteration_end();
}
