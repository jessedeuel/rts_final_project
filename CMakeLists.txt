# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(rts_final_project C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

add_subdirectory(lib/PicoLED)

# Add executable. Default name is the project name, version 0.1
add_executable(rts_final_project 
    src/rts_final_project.cpp 
    src/tasks.cpp
    src/scheduler.cpp 
    src/cal_funcs.cpp )

pico_set_program_name(rts_final_project "rts_final_project")
pico_set_program_version(rts_final_project "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(rts_final_project 0)
pico_enable_stdio_usb(rts_final_project 1)

# Add the standard library to the build
target_link_libraries(rts_final_project
        pico_stdlib
        PicoLed
        )

# Add the standard include files to the build
target_include_directories(rts_final_project PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        "./include"
        "./lib"
)

# Add any user requested libraries
target_link_libraries(rts_final_project 
        hardware_adc
        hardware_spi
        hardware_dma
        hardware_timer
        hardware_clocks
        hardware_i2c
        )

#Enable exceptions on target only
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_compile_options(${PROJECT_NAME} PRIVATE -fexceptions -Wall)

pico_add_extra_outputs(rts_final_project)

