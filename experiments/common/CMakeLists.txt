add_library(experiments
	experiments.c
	experiments.h
)

target_link_libraries(experiments
	pico_stdlib
	hardware_clocks
	hardware_pll
	hardware_xosc
	hardware_vreg
	pico_multicore
	pico_rand
	hardware_sleep
	hardware_rosc
	target_configuration
)

target_include_directories(experiments PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

add_library(target_configuration
	configurations.c
	target_configuration.c
	target_configuration.h
)

target_link_libraries(target_configuration
	pico_stdlib
	hardware_clocks
	hardware_pll
	hardware_xosc
	hardware_vreg
	pico_multicore
	pico_rand
	hardware_sleep
	hardware_rosc
)

target_include_directories(target_configuration PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
