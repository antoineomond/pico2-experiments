add_executable(experiments_run
				configs_all.c
				configs_baseline.c
				configs_minimums_pll.c
				configs_minimums_rosc.c
				configs_minimums_rosc-vregs.c
				configs_minimums_xosc.c
				configs_same_freqs-max_vco-def_vreg.c
				configs_same_freqs-max_vco-min_vreg.c
				configs_same_freqs-min_vco-def_vreg.c
				configs_same_freqs-min_vco-min_vreg.c
				configs_minimums_lposc_default_freq.c
				configs_minimums_lposc_max_freq.c
				configs_minimums_lposc_min_freq.c
        experiments_run.c)
target_link_libraries(experiments_run
				pico_stdlib
				experiments
				hardware_sleep
				hardware_pll)
pico_enable_stdio_usb(experiments_run 0)
pico_enable_stdio_uart(experiments_run 1)
pico_add_extra_outputs(experiments_run)
