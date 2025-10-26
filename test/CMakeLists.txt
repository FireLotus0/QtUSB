cmake_minimum_required(VERSION 3.2...4.0)

project(Test)

set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(QtUsb REQUIRED)

add_executable(${PROJECT_NAME}
    main.cpp
)

target_link_libraries (${PROJECT_NAME} PUBLIC QtUsb::QtUsb
)