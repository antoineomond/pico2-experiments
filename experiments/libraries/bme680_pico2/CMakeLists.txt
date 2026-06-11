add_library(bme680_pico2
	bme680_pico2.c
	bme680_pico2.h
)

target_link_libraries(bme680_pico2 pico_stdlib hardware_spi)
target_include_directories(bme680_pico2 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
