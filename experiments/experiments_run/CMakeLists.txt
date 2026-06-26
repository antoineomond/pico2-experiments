add_executable(experiments_run
        experiments_run.c)
target_link_libraries(experiments_run
				pico_stdlib
				experiments
				hardware_sleep
				hardware_pll)
pico_enable_stdio_usb(experiments_run 0)
pico_enable_stdio_uart(experiments_run 1)
pico_add_extra_outputs(experiments_run)

add_executable(experiments_run_adhoc
        experiments_run_adhoc.c)
target_link_libraries(experiments_run_adhoc
				pico_stdlib
				experiments
				hardware_sleep
				hardware_pll)
pico_enable_stdio_usb(experiments_run_adhoc 0)
pico_enable_stdio_uart(experiments_run_adhoc 1)
pico_add_extra_outputs(experiments_run_adhoc)
