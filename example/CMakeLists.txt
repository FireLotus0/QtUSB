cmake_minimum_required(VERSION 3.2...4.0)

project(Test)

add_executable(${PROJECT_NAME}
    main.cpp
    ${CMAKE_SOURCE_DIR}/src/usb_namespace.h
    ${CMAKE_SOURCE_DIR}/src/libusb.h
    ${CMAKE_SOURCE_DIR}/src/datatypes.h
    ${CMAKE_SOURCE_DIR}/src/datatypes.cpp

    ${CMAKE_SOURCE_DIR}/src/usbmonitor/usbmonitor.h
    ${CMAKE_SOURCE_DIR}/src/usbmonitor/usbmonitor.cpp
    ${CMAKE_SOURCE_DIR}/src/usbmonitor/impl/monitorbase.h
    ${CMAKE_SOURCE_DIR}/src/usbmonitor/impl/monitorbase.cpp
    ${CMAKE_SOURCE_DIR}/src/usbmonitor/impl/monitorthreaded.h
    ${CMAKE_SOURCE_DIR}/src/usbmonitor/impl/monitorthreaded.cpp
)

target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    ${USB_LIB}
)