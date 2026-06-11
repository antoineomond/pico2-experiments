add_library(sx126x_driver
	lr_fhss_mac.c
	lr_fhss_mac.h
	lr_fhss_v1_base_types.h
	sx126x.c
	sx126x.h
	sx126x_driver_version.c
	sx126x_driver_version.h
	sx126x_hal.c
	sx126x_hal.h
	sx126x_lr_fhss.c
	sx126x_lr_fhss.h
	sx126x_regs.h
	printers/sx126x_str.c
	printers/sx126x_str.h
	configuration/apps_common.c
	configuration/apps_common.h
	configuration/apps_configuration.h
	configuration/apps_utilities.h
)

target_link_libraries(sx126x_driver pico_stdlib hardware_spi sx1262_wvshre_pico2)
target_include_directories(sx126x_driver PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
