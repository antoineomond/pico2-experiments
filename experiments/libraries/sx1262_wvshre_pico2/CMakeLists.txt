add_library(sx1262_wvshre_pico2
	common.c
	common.h
)

target_link_libraries(sx1262_wvshre_pico2 pico_stdlib hardware_spi sx126x_driver)
target_include_directories(sx1262_wvshre_pico2 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
