add_executable(${NAME}
        bus_scan.c
        )

# pull in common dependencies and additional i2c hardware support
target_link_libraries(${NAME} pico_stdlib hardware_i2c)

# create map/bin/hex file etc.
pico_add_extra_outputs(${NAME})

pico_enable_stdio_usb(${NAME} 0)
pico_enable_stdio_uart(${NAME} 1)
target_compile_definitions(${NAME} PRIVATE
    PICO_DEFAULT_UART_RX_PIN=16
    PICO_DEFAULT_UART_TX_PIN=17
)

