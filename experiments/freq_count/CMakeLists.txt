add_executable(freq_count_rosc
        freq_count_rosc.c)
target_link_libraries(freq_count_rosc
				pico_stdlib
        hardware_clocks
				hardware_rosc
				hardware_vreg
				hardware_clocks
				hardware_pll
				hardware_sleep)
pico_enable_stdio_usb(freq_count_rosc 0)
pico_enable_stdio_uart(freq_count_rosc 1)
pico_add_extra_outputs(freq_count_rosc)

add_executable(freq_count_lposc
        freq_count_lposc.c)
target_link_libraries(freq_count_lposc
				pico_stdlib
        hardware_clocks
				hardware_rosc
				hardware_vreg
				hardware_clocks
				hardware_pll
				hardware_sleep)
pico_enable_stdio_usb(freq_count_lposc 0)
pico_enable_stdio_uart(freq_count_lposc 1)
pico_add_extra_outputs(freq_count_lposc)
