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
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(Omnibus_Screen C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(Omnibus_Screen src/Omnibus_Screen.c
               src/driver_HUB75E.c
               src/frame_assembly.c
               src/data_font_5x7.c
               src/driver_timer.c
               src/driver_buttons.c
               src/driver_rotary_encoder.c
               src/service_scheduler.c
               src/interface_buttons.c
               src/interface_rotary_encoder.c
               src/process_button_actions.c
               src/process_rotary_encoder_actions.c
               src/process_UI.c
               src/data_UI.c
)

target_compile_options(Omnibus_Screen PRIVATE -Wall -Wextra -Wpedantic)

pico_set_program_name(Omnibus_Screen "Omnibus_Screen")
pico_set_program_version(Omnibus_Screen "0.1")

# Generate PIO header
pico_generate_pio_header(Omnibus_Screen ${CMAKE_CURRENT_LIST_DIR}/hub75.pio)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(Omnibus_Screen 0)
pico_enable_stdio_usb(Omnibus_Screen 1)

# Add the standard library to the build
target_link_libraries(Omnibus_Screen
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(Omnibus_Screen PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/inc

)

# Add any user requested libraries
target_link_libraries(Omnibus_Screen 
        hardware_i2c
        hardware_dma
        hardware_pio
        hardware_timer
        
        )

pico_add_extra_outputs(Omnibus_Screen)
