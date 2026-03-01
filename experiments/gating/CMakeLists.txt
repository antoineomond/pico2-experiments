add_executable(gating
        gating.c)
target_link_libraries(gating
				pico_stdlib
        hardware_clocks
				experiments)
pico_enable_stdio_usb(gating 0)
pico_enable_stdio_uart(gating 1)
pico_add_extra_outputs(gating)
