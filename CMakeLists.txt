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
set(PICO_BOARD pico2 CACHE STRING "Board type")

cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(epaper_touch C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


add_executable(epaper_touch
    examples/main.c
    examples/2in13_TestCode.c
    examples/2in13_TestCode_V3.c
    examples/2in13_TestCode_V4.c
    examples/2in9_TestCode.c
    
    lib/Config/DEV_Config.c
    
    lib/Driver/GT1151.c
    lib/Driver/ICNT86X.c
    
    lib/EPD/EPD_2in13_V4.c
    lib/EPD/EPD_2in13_V3.c
    lib/EPD/EPD_2in13_V2.c
    lib/EPD/EPD_2in9_V2.c
    
    lib/GUI/GUI_Paint.c
    
    lib/Fonts/font8.c
    lib/Fonts/font12.c
    lib/Fonts/font16.c
    lib/Fonts/font20.c
    lib/Fonts/font24.c
    lib/Fonts/font12CN.c
    lib/Fonts/font24CN.c
)

target_include_directories(epaper_touch PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/Config
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/Driver
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/EPD
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/Fonts
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/GUI
    ${CMAKE_CURRENT_SOURCE_DIR}/examples
)

target_link_libraries(epaper_touch
    pico_stdlib
    hardware_spi
    hardware_i2c
    hardware_gpio
)

# Enable USB serial output for debugging, disable UART
pico_enable_stdio_usb(epaper_touch 1)
pico_enable_stdio_uart(epaper_touch 0)

pico_add_extra_outputs(epaper_touch)
