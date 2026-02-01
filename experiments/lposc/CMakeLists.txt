add_executable(lposc_benchmarks
        lposc_benchmarks.c)
target_link_libraries(lposc_benchmarks
				pico_stdlib
        hardware_clocks
				hardware_pll
				hardware_xosc
				hardware_vreg
				pico_multicore
				pico_rand
				hardware_sleep)
pico_enable_stdio_usb(lposc_benchmarks 0)
pico_enable_stdio_uart(lposc_benchmarks 1)
pico_add_extra_outputs(lposc_benchmarks)
