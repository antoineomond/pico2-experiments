#include <stdint.h>
#include "sx126x.h"

#ifndef DIO1_PIN
#define DIO1_PIN 20
#endif
#ifndef DIO2_PIN
#define DIO2_PIN 21
#endif
#ifndef BUSY_PIN
#define BUSY_PIN 8
#endif
#ifndef RESET_PIN
#define RESET_PIN 9
#endif
#ifndef FREQ_HZ
#define FREQ_HZ 920000000
#endif
#ifndef RX_BUF_ADDRESS
#define RX_BUF_ADDRESS 0x0F
#endif
#ifndef TX_BUF_ADDRESS
#define TX_BUF_ADDRESS 0x0
#endif
#ifndef NB_ITERATIONS
#define NB_ITERATIONS 5000
#endif

typedef struct
{
	uint8_t spio_rx;  // connect to MISO
	uint8_t spio_csn; // connect to CS (NSS)
	uint8_t spio_sck; // connect to CLK
	uint8_t spio_tx;  // connect to MOSI
} spi_connection;

typedef struct
{
	uint8_t power;		 // pow
	uint8_t ramp_time; // connect to CS (NSS)
} sx1262_tx_params;

#define SX1262_GET_STATUS( fc, fname )                                                                  \
    {                                                                                            \
			ASSERT_SX126X_RC(fc);                                                                                        \
			spi_connection* context = malloc(sizeof(spi_connection));                                  \
			context->spio_rx = PICO_DEFAULT_SPI_RX_PIN;                                           \
			context->spio_csn = PICO_DEFAULT_SPI_CSN_PIN;                                         \
			context->spio_sck = PICO_DEFAULT_SPI_SCK_PIN;                                         \
			context->spio_tx = PICO_DEFAULT_SPI_TX_PIN;                                           \
			sx126x_chip_status_t* radio_status = malloc(sizeof(sx126x_chip_status_t));							   \
			sleep_us((int)(TIME_RATE*100000));                                                                             \
			ASSERT_SX126X_RC(sx126x_get_status(context, radio_status));																 \
			sleep_us((int)(TIME_RATE*50000));                                                                             \
			printf("chip_mode (%s): %d\n", fname, radio_status->chip_mode);				 \
			printf("cmd_status (%s): %d\n", fname, radio_status->cmd_status);			 \
    }

extern float TIME_RATE;
void wait_sx1262_busy();
void pico2_spi_init_default(uint32_t baud_rate);
void sx1262_pico2_init(void* dio_gpio_callback);
void sx1262_lora_init(const void* context, uint16_t irq_mask);
void print_irq_to_str(sx126x_irq_mask_t irq_mask);
