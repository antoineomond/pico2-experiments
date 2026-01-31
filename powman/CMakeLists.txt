add_executable(powman_lowest_consumption
        powman_lowest_consumption.c)
target_link_libraries(powman_lowest_consumption
				pico_stdlib
        hardware_clocks
        hardware_powman
				hardware_watchdog
				hardware_sleep)
pico_enable_stdio_usb(powman_lowest_consumption 0)
pico_enable_stdio_uart(powman_lowest_consumption 1)
pico_add_extra_outputs(powman_lowest_consumption)

