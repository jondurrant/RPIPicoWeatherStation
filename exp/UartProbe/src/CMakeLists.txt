add_executable(${NAME}
        main.cpp
        )

# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(${NAME} 
    pico_stdlib

    )

target_include_directories(${NAME} PRIVATE 
	../../../src/
	$ENV{PICO_EXTRAS_PATH}/src/rp2_common/pico_sleep/include
	)
	

# create map/bin/hex file etc.
pico_add_extra_outputs(${NAME})

pico_enable_stdio_usb(${NAME} 1)
pico_enable_stdio_uart(${NAME} 0)
