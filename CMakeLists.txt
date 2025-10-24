cmake_minimum_required(VERSION 3.2...4.0)

project(QtUSB)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

if(MSVC)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif ()

find_package(Qt5 COMPONENTS Core REQUIRED)
find_library(USB_LIB NAMES libusb-1.0 usb-1.0)
if (NOT USB_LIB)
    message(FATAL_ERROR "can`t find libusb")
endif ()

add_subdirectory(example)