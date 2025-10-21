cmake_minimum_required(VERSION 3.2...4.0)

project(Test)

add_executable(${PROJECT_NAME}
    main.cpp
    ${CMAKE_SOURCE_DIR}/src/usb_namespace.h
    ${CMAKE_SOURCE_DIR}/src/datatypes.h

    ${CMAKE_SOURCE_DIR}/src/usbmonitor/usbmonitor.h
    ${CMAKE_SOURCE_DIR}/src/usbmonitor/usbmonitor.cpp
    ${CMAKE_SOURCE_DIR}/src/usbmonitor/impl/monitorbase.h
    ${CMAKE_SOURCE_DIR}/src/usbmonitor/impl/monitorbase.cpp

    ${CMAKE_SOURCE_DIR}/src/usbmonitor/impl/monitorwin.h
    ${CMAKE_SOURCE_DIR}/src/usbmonitor/impl/monitorwin.cpp

)

target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    ${USB_LIB}
)