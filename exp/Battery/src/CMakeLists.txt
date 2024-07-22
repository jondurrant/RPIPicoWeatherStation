add_executable(${NAME}
        main.cpp
        "$ENV{PICO_EXAMPLES_PATH}/adc/read_vsys/power_status.c"
        )

# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(${NAME} 
    pico_stdlib
	pico_cyw43_arch_none
	hardware_adc
    )

target_include_directories(${NAME} PUBLIC
	"$ENV{PICO_EXAMPLES_PATH}/adc/read_vsys/"
)

# create map/bin/hex file etc.
pico_add_extra_outputs(${NAME})

pico_enable_stdio_usb(${NAME} 0)
pico_enable_stdio_uart(${NAME} 1)
target_compile_definitions(${NAME} PRIVATE
    PICO_DEFAULT_UART_RX_PIN=16
    PICO_DEFAULT_UART_TX_PIN=17
)

