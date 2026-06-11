add_executable(lora_spi_sender
        lora_spi_sender.c)
target_link_libraries(lora_spi_sender
				pico_stdlib
				bme680_pico2
				bme68x_driver
				experiments_bis
				sx1262_wvshre_pico2
				sx126x_driver
				experiments
				hardware_sleep
				hardware_pll)
pico_enable_stdio_usb(lora_spi_sender 1)
pico_enable_stdio_uart(lora_spi_sender 0)
pico_add_extra_outputs(lora_spi_sender)

add_executable(lora_spi_receiver
        lora_spi_receiver.c)
target_link_libraries(lora_spi_receiver
				pico_stdlib
				bme680_pico2
				bme68x_driver
				experiments_bis
				sx1262_wvshre_pico2
				sx126x_driver
				experiments
				hardware_sleep
				hardware_pll)
pico_enable_stdio_usb(lora_spi_receiver 1)
pico_enable_stdio_uart(lora_spi_receiver 0)
pico_add_extra_outputs(lora_spi_receiver)
