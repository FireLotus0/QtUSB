cmake_minimum_required(VERSION 3.2...4.0)

project(Test)

include_directories(${CMAKE_SOURCE_DIR}/include/)
include_directories(${CMAKE_SOURCE_DIR}/src/)

find_package(Qt5 COMPONENTS Core REQUIRED)

if (MSVC)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif ()

add_executable(${PROJECT_NAME}
    main.cpp
    ${QtUsb_HEADERS}
    ${QtUsb_SOURCES}
)

target_link_libraries(Test
    Qt5::Core
    QtUsb
)
