add_executable(experiments_run
				params_baseline.c
				params_minimums.c
				params_minimums_rosc.c
				params_minimums_xosc.c
				params_same_freqs.c
        experiments_run.c)
target_link_libraries(experiments_run
				pico_stdlib
				experiments
				hardware_sleep
				hardware_pll)
pico_enable_stdio_usb(experiments_run 0)
pico_enable_stdio_uart(experiments_run 1)
pico_add_extra_outputs(experiments_run)
