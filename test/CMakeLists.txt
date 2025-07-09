set(TEST_NAME "test")
set(PICO_CXX_ENABLE_EXCEPTIONS 1)

include(${FREERTOS_CONFIG_FILE_DIRECTORY}/freertos_unit.cmake)

# CppUTest
include(FetchContent)
FetchContent_Declare(
    CppUTest
    GIT_REPOSITORY https://github.com/cpputest/cpputest.git
    GIT_TAG        master # or use release tag, eg. v4.0
)
# Set this to ON if you want to have the CppUTests in your project as well.
set(TESTS OFF CACHE BOOL "Switch off CppUTest Test build")
set(CPPUTEST_STD_CPP_LIB_DISABLED OFF CACHE BOOL "OFF")
set(CPPUTEST_PLATFORM OFF CACHE BOOL "Providing FreeRTOS platform")
FetchContent_MakeAvailable(CppUTest)

include(../src/files.cmake)
add_executable(${TEST_NAME}
    test_main.cpp
    dummy.cpp
    testRGB.cpp
    testRain.cpp
    testAnem.cpp
    testVane.cpp
    testCtr.cpp
    testAHT10.cpp
    testDS3231.cpp
    testPico.cpp
    testSen500.cpp
    testPayload.cpp
    
    UtestPlatform.cpp
    
    ${SRC_FILES}
)

# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(${TEST_NAME} 
	 pico_stdlib
     FreeRTOS-Kernel-Heap4 # FreeRTOS kernel and dynamic heap
     freertos_unit #FREERTOS_PORT
     CppUTest::CppUTest
     CppUTest::CppUTestExt
     
     hardware_adc
	 hardware_pwm
	 hardware_i2c
	 AHTXX
	 dormant
	 LWIP_PORT
	 pico_cyw43_arch_lwip_sys_freertos
	 
	 coreHTTP
     urilib
     json_maker
     tiny_json
     wolfssl
     picow_wsclient
     nvs_onboard
     
     
	 
)
	
target_include_directories(${TEST_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/../src
)
     

target_compile_definitions(${TEST_NAME} PRIVATE
    PICO_CXX_DISABLE_ALLOCATION_OVERRIDES
    CPPUTEST_HAVE_EXCEPTIONS=1
)

# create map/bin/hex file etc.
pico_add_extra_outputs(${TEST_NAME})

# enable usb output, disable uart output
pico_enable_stdio_usb(${TEST_NAME} 0)
pico_enable_stdio_uart(${TEST_NAME} 1)
target_compile_definitions(${TEST_NAME} PRIVATE
    PICO_DEFAULT_UART_RX_PIN=17
    PICO_DEFAULT_UART_TX_PIN=16
)