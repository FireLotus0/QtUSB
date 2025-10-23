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
    ${CMAKE_SOURCE_DIR}/src/usbmonitor/impl/monitorhotplug.h
    ${CMAKE_SOURCE_DIR}/src/usbmonitor/impl/monitorhotplug.cpp

    ${CMAKE_SOURCE_DIR}/src/descriptor/usbdescriptor.h
    ${CMAKE_SOURCE_DIR}/src/descriptor/usbdescriptor.cpp
    ${CMAKE_SOURCE_DIR}/src/descriptor/descriptorbase/descriptordata.h
    ${CMAKE_SOURCE_DIR}/src/descriptor/descriptorbase/descriptordata.cpp
    ${CMAKE_SOURCE_DIR}/src/descriptor/descriptorbase/descriptorbase.h
    ${CMAKE_SOURCE_DIR}/src/descriptor/descriptorbase/descriptorbase.cpp
    ${CMAKE_SOURCE_DIR}/src/descriptor/device/devicedesc.h
    ${CMAKE_SOURCE_DIR}/src/descriptor/device/devicedesc.cpp
    ${CMAKE_SOURCE_DIR}/src/descriptor/configuration/configdesc.h
    ${CMAKE_SOURCE_DIR}/src/descriptor/configuration/configdesc.cpp
    ${CMAKE_SOURCE_DIR}/src/descriptor/interface/interfacedesc.h
    ${CMAKE_SOURCE_DIR}/src/descriptor/interface/interfacedesc.cpp
    ${CMAKE_SOURCE_DIR}/src/descriptor/interface/interfacealter.h
    ${CMAKE_SOURCE_DIR}/src/descriptor/interface/interfacealter.cpp
    ${CMAKE_SOURCE_DIR}/src/descriptor/endpoint/endpointdesc.h
    ${CMAKE_SOURCE_DIR}/src/descriptor/endpoint/endpointdesc.cpp
)

target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    ${USB_LIB}
)