add_executable(baseline
        baseline.c)
target_link_libraries(baseline
				pico_stdlib
				experiments
				hardware_sleep
				hardware_pll)
pico_enable_stdio_usb(baseline 0)
pico_enable_stdio_uart(baseline 1)
pico_add_extra_outputs(baseline)
