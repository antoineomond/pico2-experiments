add_executable(pll_pairs_freqs
        pll_pairs_freqs.c)
target_link_libraries(pll_pairs_freqs
				pico_stdlib
				experiments
				hardware_pll)
pico_enable_stdio_usb(pll_pairs_freqs 0)
pico_enable_stdio_uart(pll_pairs_freqs 1)
pico_add_extra_outputs(pll_pairs_freqs)

add_executable(rosc_pairs_freqs
        rosc_pairs_freqs.c)
target_link_libraries(rosc_pairs_freqs
				pico_stdlib
        hardware_clocks
				hardware_pll
				hardware_xosc
				hardware_vreg
				pico_multicore
				pico_rand
				experiments
				hardware_sleep)
pico_enable_stdio_usb(rosc_pairs_freqs 0)
pico_enable_stdio_uart(rosc_pairs_freqs 1)
pico_add_extra_outputs(rosc_pairs_freqs)

add_executable(rosc_pairs_freqs_manual
        rosc_pairs_freqs_manual.c)
target_link_libraries(rosc_pairs_freqs_manual
				pico_stdlib
        hardware_clocks
				hardware_pll
				hardware_xosc
				hardware_vreg
				pico_multicore
				pico_rand
				experiments
				hardware_sleep)
pico_enable_stdio_usb(rosc_pairs_freqs_manual 0)
pico_enable_stdio_uart(rosc_pairs_freqs_manual 1)
pico_add_extra_outputs(rosc_pairs_freqs_manual)
