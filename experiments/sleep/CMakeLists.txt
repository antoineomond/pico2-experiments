add_executable(sleep_wfi
        sleep_wfi.c)
target_link_libraries(sleep_wfi
				pico_stdlib
        hardware_clocks
				hardware_powman
				hardware_watchdog
				hardware_pll
				hardware_xosc
				hardware_vreg
				hardware_sleep)
pico_enable_stdio_usb(sleep_wfi 0)
pico_enable_stdio_uart(sleep_wfi 1)
pico_add_extra_outputs(sleep_wfi)

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

