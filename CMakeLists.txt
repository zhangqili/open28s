cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME open28s)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Add libamp sources
set(LIBAMP_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/libamp_user)
add_subdirectory(libamp)
add_subdirectory(u8g2)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

file(GLOB_RECURSE MY_SOURCES
    "./CherryUSB/core/usbd_core.c"
    "./CherryUSB/class/hid/usbd_hid.c"
    "./CherryUSB/port/fsdev/usb_dc_fsdev.c"
    "./CherryUSB/port/fsdev/usb_glue_st.c"
    "./usb/*.c"
    "./libamp_user/*.c"
    "./libamp/usb/template/cherryusb/usbd_user.c"
    "./fezui/src/*.c"
    "./fezui/ui/*.c"
    "./snake/*.c")

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    ${MY_SOURCES}
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    ./usb/
    ./CherryUSB/core
    ./CherryUSB/common
    ./CherryUSB/class/hid
    ./CherryUSB/class/midi
    ./CherryUSB/port/fsdev
    ./Drivers/CMSIS/Device/ST/STM32F3xx/Include
    ./libamp/usb/template/cherryusb
    ./fezui/src
    ./fezui/ui
    ./snake
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
    u8g2
    libamp
    m
)
