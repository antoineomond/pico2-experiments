add_library(experiments_bis
	experiments.c
	experiments.h
)

target_link_libraries(experiments_bis pico_stdlib hardware_pll hardware_spi hardware_clocks hardware_xosc pico_aon_timer hardware_rosc hardware_sleep pico_stdio_uart hardware_ticks pico_rand)
target_include_directories(experiments_bis PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
