//#ifndef PICO_STDIO_USB_CONNECT_WAIT_TIMEOUT_MS         https://stackoverflow.com/questions/76004473/why-does-printf-work-in-the-while-loop-but-not-before-on-a-raspberry-pi-pico
//#define PICO_STDIO_USB_CONNECT_WAIT_TIMEOUT_MS (1000)
//#endif

#include "sx126x.h"
#include "sx126x_hal.h"
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"
#include "configuration/apps_configuration.h"
#include "configuration/apps_common.h"
#include "configuration/apps_utilities.h"

const sx126x_mod_params_lora_t lora_mod_params = {
    .sf   = LORA_SPREADING_FACTOR,
    .bw   = LORA_BANDWIDTH,
    .cr   = LORA_CODING_RATE,
    .ldro = 0x01,  // Will be initialized during radio init
};

const sx126x_pkt_params_lora_t lora_pkt_params = {
    .preamble_len_in_symb = LORA_PREAMBLE_LENGTH,
    .header_type          = LORA_PKT_LEN_MODE,
    .pld_len_in_bytes     = PAYLOAD_LENGTH,
    .crc_is_on            = LORA_CRC,
    .invert_iq_is_on      = LORA_IQ,
};
bool is_sx1262_busy = false;
uint32_t callback_event_mask = GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL;
extern float TIME_RATE;


void busy_pin_callback()
{
	if (gpio_get_irq_event_mask(BUSY_PIN) & GPIO_IRQ_EDGE_RISE) {
	 	gpio_acknowledge_irq(BUSY_PIN, GPIO_IRQ_EDGE_RISE);
		is_sx1262_busy = true;
	}
	if (gpio_get_irq_event_mask(BUSY_PIN) & GPIO_IRQ_EDGE_FALL) {
	 	gpio_acknowledge_irq(BUSY_PIN, GPIO_IRQ_EDGE_FALL);
		is_sx1262_busy = false;
	}
}

void wait_sx1262_busy() {
	//printf("is sx1262 busy: %d\n", is_sx1262_busy);
	while(is_sx1262_busy) {
		//printf("sx1262 is busy\n");
		sleep_us((int)(TIME_RATE*1000000));
	}
}

void pico2_spi_init_default(uint32_t baud_rate) {
	uint32_t real_baud_rate;
	real_baud_rate = spi_init(spi_default, baud_rate); // notably set baud rate TODO check
	//printf("real_baud_rate: %d\n", real_baud_rate);
	gpio_set_function(PICO_DEFAULT_SPI_RX_PIN, GPIO_FUNC_SPI);
	gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
	gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);
	// Make the SPI pins available to picotool, informative only
	//bi_decl(bi_3pins_with_func(PICO_DEFAULT_SPI_RX_PIN, PICO_DEFAULT_SPI_TX_PIN, PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI));
	//printf("gpio csn pin\n");

	// Chip select is active-low, so we'll initialise it to a driven-high state
	gpio_init(PICO_DEFAULT_SPI_CSN_PIN);
	gpio_set_dir(PICO_DEFAULT_SPI_CSN_PIN, GPIO_OUT);
	// Make the CS pin available to picotool
	//bi_decl(bi_1pin_with_name(PICO_DEFAULT_SPI_CSN_PIN, "SPI CS"));
}

void sx1262_pico2_init(void* dio_gpio_callback) {
	gpio_init(DIO1_PIN);
	gpio_set_dir(DIO1_PIN, GPIO_IN);
	gpio_init(BUSY_PIN);
	gpio_set_dir(BUSY_PIN, GPIO_IN);
	gpio_init(RESET_PIN);
	gpio_set_dir(RESET_PIN, GPIO_OUT);
  gpio_set_irq_enabled_with_callback(DIO1_PIN, callback_event_mask, true, dio_gpio_callback);
	gpio_set_irq_enabled(BUSY_PIN, callback_event_mask, true);
  gpio_add_raw_irq_handler(BUSY_PIN, &busy_pin_callback);
}

void print_irq_to_str(sx126x_irq_mask_t irq_mask) {
	if(irq_mask & (1 << 0)) fwrite("TxDone ", 1, 7, stdout); sleep_us((int)(TIME_RATE*50000));
	if(irq_mask & (1 << 1)) fwrite("RxDone ", 1, 7, stdout); sleep_us((int)(TIME_RATE*50000));
	if(irq_mask & (1 << 2)) fwrite("PreambleDetected ", 1, 17, stdout); sleep_us((int)(TIME_RATE*50000));
	if(irq_mask & (1 << 3)) fwrite("SyncWordValid ", 1, 14, stdout); sleep_us((int)(TIME_RATE*50000));
	if(irq_mask & (1 << 4)) fwrite("HeaderValid ", 1, 12, stdout); sleep_us((int)(TIME_RATE*50000));
	if(irq_mask & (1 << 5)) fwrite("HeaderErr ", 1, 10, stdout); sleep_us((int)(TIME_RATE*50000));
	if(irq_mask & (1 << 6)) fwrite("CrcErr ", 1, 7, stdout); sleep_us((int)(TIME_RATE*50000));
	if(irq_mask & (1 << 7)) fwrite("CadDone ", 1, 8, stdout); sleep_us((int)(TIME_RATE*50000));
	if(irq_mask & (1 << 8)) fwrite("CadDetected ", 1, 12, stdout); sleep_us((int)(TIME_RATE*50000));
	if(irq_mask & (1 << 9)) fwrite("Timeout ", 1, 8, stdout); sleep_us((int)(TIME_RATE*50000));
	fwrite("\n", 1, 1, stdout);
	sleep_us((int)(TIME_RATE*50000));
}

void sx1262_lora_init(const void* context, uint16_t irq_mask) {
	// TODO: check for busy pin before each action (macro?)
	// Reset board
	//printf("reset board\n");
  gpio_put(RESET_PIN, 0);
	sleep_us((int)(TIME_RATE*200000));
  gpio_put(RESET_PIN, 1);
	sleep_us((int)(TIME_RATE*1000000));
	wait_sx1262_busy();
	SX1262_GET_STATUS(sx126x_set_standby(context, SX126X_STANDBY_CFG_RC), "sx126x_set_standby");
	wait_sx1262_busy();
	SX1262_GET_STATUS(sx126x_set_pkt_type(context, SX126X_PKT_TYPE_LORA), "sx126x_set_pkt_type");
	wait_sx1262_busy();
	SX1262_GET_STATUS(sx126x_set_rf_freq(context, FREQ_HZ), "sx126x_set_rf_freq");
	wait_sx1262_busy();
	SX1262_GET_STATUS(sx126x_set_lora_mod_params(context, &lora_mod_params), "sx126x_set_lora_mod_params");
	wait_sx1262_busy();
	SX1262_GET_STATUS(sx126x_set_lora_pkt_params(context, &lora_pkt_params), "sx126x_set_lora_pkt_params");
	wait_sx1262_busy();
	SX1262_GET_STATUS(sx126x_set_buffer_base_address(context, TX_BUF_ADDRESS, RX_BUF_ADDRESS), "sx126x_set_buffer_base_address");

	// Configure DIO and IRQ
	uint16_t dio1_mask = irq_mask; // Need to correspond to irq mask
	uint16_t dio2_mask = 0;				 // Not using dio2 for now
	uint16_t dio3_mask = 0;				 // Not using dio2 for now
	wait_sx1262_busy();
	SX1262_GET_STATUS(sx126x_set_dio_irq_params(context, irq_mask, dio1_mask, dio2_mask, dio3_mask), "sx126x_set_dio_irq_params");
	wait_sx1262_busy();
	SX1262_GET_STATUS(sx126x_set_rx_tx_fallback_mode(context, FALLBACK_MODE), "sx126x_set_rx_tx_fallback_mode");
	wait_sx1262_busy();
	SX1262_GET_STATUS(sx126x_set_lora_sync_word(context, LORA_SYNCWORD), "sx126x_set_lora_sync_word"); // (SWSD003)
	wait_sx1262_busy();
	SX1262_GET_STATUS(sx126x_clear_device_errors(context), "sx126x_clear_device_errors");
	wait_sx1262_busy();
	SX1262_GET_STATUS(sx126x_reset_stats(context), "sx126x_reset_stats");
	//wait_sx1262_busy();
	//SX1262_GET_STATUS(sx126x_set_dio2_as_rf_sw_ctrl(context, true), "sx126x_set_dio2_as_rf_sw_ctrl");
	wait_sx1262_busy();
	SX1262_GET_STATUS(sx126x_set_dio3_as_tcxo_ctrl(context, SX126X_TCXO_CTRL_3_3V, 300), "sx126x_set_dio3_as_tcxo_ctrl");
}
