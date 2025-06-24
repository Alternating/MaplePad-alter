cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

# Enable C and C++ (SDK requires both, but we'll only use C)
project(maplepad C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Set board type
set(PICO_BOARD pico)

# Disable C++ features we don't want
set(PICO_CXX_ENABLE_EXCEPTIONS 0)
set(PICO_CXX_ENABLE_RTTI 0)

pico_sdk_init()

add_executable(maplepad)

target_compile_definitions(maplepad PRIVATE 
    PICO_HW
    PICO_RP2350=1
    PICO_NO_HARDWARE_EXCEPTION=1
)

pico_add_extra_outputs(maplepad)

pico_generate_pio_header(maplepad ${CMAKE_CURRENT_LIST_DIR}/src/maple.pio)

target_sources(maplepad PRIVATE 
    src/maple.c 
    src/state_machine.c 
    src/format.c 
    src/display.c 
    src/ssd1331.c 
    src/ssd1306.c 
    src/ssd1309.c
    src/font.c 
    src/menu.c
    src/sdcard.c
)

target_link_libraries(maplepad PRIVATE
        pico_stdlib
        pico_multicore
        pico_time
        hardware_adc
        hardware_pio
        hardware_dma
        hardware_pwm
        hardware_timer
        hardware_i2c
        hardware_spi
        hardware_flash
        )

# Force C-only compilation for our source files
set_source_files_properties(
    src/maple.c 
    src/state_machine.c 
    src/format.c 
    src/display.c 
    src/ssd1331.c 
    src/ssd1306.c 
    src/ssd1309.c
    src/font.c 
    src/menu.c
    src/sdcard.c
    PROPERTIES 
    LANGUAGE C
)