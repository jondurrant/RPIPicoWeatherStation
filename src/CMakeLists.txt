add_executable(${NAME}
        main.cpp
        
        Anemometer.cpp
        AnemometerPWM.cpp
        WindVane.cpp
        Rain.cpp
        Sen0500.cpp
        
        PowerCtr.cpp
        
        GPIOInputMgr.cpp
        GPIOObserver.cpp
        
        Agent.cpp
        StatusAgent.cpp
        DeepSleepRTOS.cpp
        
        )

# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(${NAME} 
    pico_stdlib
	hardware_adc
	hardware_pwm
	hardware_i2c
	AHTXX
	dormant
	
	FreeRTOS-Kernel-Heap4 # FreeRTOS kernel and dynamic heap
    freertos_config #FREERTOS_PORT
    LWIP_PORT
    pico_cyw43_arch_lwip_sys_freertos
    coreHTTP
    urilib
    json_maker
    tiny_json
    wolfssl
    picow_wsclient
	)

target_include_directories(${NAME} PRIVATE 
	../../../src/
	$ENV{PICO_EXTRAS_PATH}/src/rp2_common/pico_sleep/include
	)
	
	
if (NOT DEFINED ENV{WIFI_SSID})
	message(FATAL_ERROR "WIFI_SSID Environment variable not defined")
endif()
if (NOT DEFINED ENV{WIFI_PASSWORD})
	message(FATAL_ERROR "WIFI_PASSWORD Environment variable not defined")
endif()

target_compile_definitions(${NAME} PRIVATE
    WIFI_SSID=\"$ENV{WIFI_SSID}\"
    WIFI_PASSWORD=\"$ENV{WIFI_PASSWORD}\"
    CYW43_HOST_NAME="WeatherStation"
)

# create map/bin/hex file etc.
pico_add_extra_outputs(${NAME})

pico_enable_stdio_uart(${NAME} 1)
target_compile_definitions(${NAME} PRIVATE
    PICO_DEFAULT_UART_RX_PIN=16
    PICO_DEFAULT_UART_TX_PIN=17
)

