add_executable(select_conf
				select_conf.c)
target_link_libraries(select_conf
				pico_stdlib
				target_configuration
				hardware_sleep)
pico_enable_stdio_usb(select_conf 0)
pico_enable_stdio_uart(select_conf 1)
pico_add_extra_outputs(select_conf)

