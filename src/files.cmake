set(SRC_FILES
        ${CMAKE_CURRENT_LIST_DIR}/Anemometer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/AnemometerPWM.cpp
        ${CMAKE_CURRENT_LIST_DIR}/WindVane.cpp
        ${CMAKE_CURRENT_LIST_DIR}/Rain.cpp
        ${CMAKE_CURRENT_LIST_DIR}/Sen0500.cpp
        
        ${CMAKE_CURRENT_LIST_DIR}/PowerCtr.cpp
        
        ${CMAKE_CURRENT_LIST_DIR}/GPIOInputMgr.cpp
        ${CMAKE_CURRENT_LIST_DIR}/GPIOObserver.cpp
        
        ${CMAKE_CURRENT_LIST_DIR}/Agent.cpp
        ${CMAKE_CURRENT_LIST_DIR}/StatusAgent.cpp
        ${CMAKE_CURRENT_LIST_DIR}/DeepSleepRTOS.cpp
        ${CMAKE_CURRENT_LIST_DIR}/VoltMeter.cpp
        ${CMAKE_CURRENT_LIST_DIR}/PicoStatus.cpp
        ${CMAKE_CURRENT_LIST_DIR}/SDLog.cpp
        ${CMAKE_CURRENT_LIST_DIR}/hw_config.c
        
        ${CMAKE_CURRENT_LIST_DIR}/WeatherStationPayload.cpp
        ${CMAKE_CURRENT_LIST_DIR}/PayloadPart.cpp
        ${CMAKE_CURRENT_LIST_DIR}/WeatherStation.cpp
        ${CMAKE_CURRENT_LIST_DIR}/AHT10Status.cpp
        
        "$ENV{PICO_EXAMPLES_PATH}/adc/read_vsys/power_status.c"
)


include_directories(AFTER
		"$ENV{PICO_EXAMPLES_PATH}/adc/read_vsys/"
)