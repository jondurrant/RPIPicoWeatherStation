# Add library cpp files

if (NOT DEFINED AHTXX_DIR)
    set(AHTXX_DIR "${CMAKE_CURRENT_LIST_DIR}/lib/AHTXX_PICO")
endif()

add_library(AHTXX STATIC)
target_sources(AHTXX PUBLIC
    ${AHTXX_DIR}/src/ahtxx/ahtxx.cpp
)

# Add include directory
target_include_directories(AHTXX PUBLIC 
   ${AHTXX_DIR}/include/
)

# Add the standard library to the build
target_link_libraries(AHTXX PUBLIC 
    pico_stdlib
    hardware_i2c
)