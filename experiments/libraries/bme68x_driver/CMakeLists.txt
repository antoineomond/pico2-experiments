add_library(bme68x_driver
	bme68x.c
	bme68x.h
	bme68x_defs.h
)

target_include_directories(bme68x_driver PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
