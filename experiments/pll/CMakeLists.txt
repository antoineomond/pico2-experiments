add_executable(pll_pairs_freqs
        pll_pairs_freqs.c)
target_link_libraries(pll_pairs_freqs
				pico_stdlib
				hardware_pll)
pico_enable_stdio_usb(pll_pairs_freqs 0)
pico_enable_stdio_uart(pll_pairs_freqs 1)
pico_add_extra_outputs(pll_pairs_freqs)
