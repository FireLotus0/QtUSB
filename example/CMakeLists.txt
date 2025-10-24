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

    ${CMAKE_SOURCE_DIR}/src/transfer/transfercontext.h
    ${CMAKE_SOURCE_DIR}/src/transfer/transfercontext.cpp
    ${CMAKE_SOURCE_DIR}/src/transfer/impl/strategybase.h
    ${CMAKE_SOURCE_DIR}/src/transfer/impl/strategybase.cpp
    ${CMAKE_SOURCE_DIR}/src/transfer/impl/sync/bulk/syncbulktransfer.h
    ${CMAKE_SOURCE_DIR}/src/transfer/impl/sync/bulk/syncbulktransfer.cpp
    ${CMAKE_SOURCE_DIR}/src/transfer/impl/sync/control/synccontroltransfer.h
    ${CMAKE_SOURCE_DIR}/src/transfer/impl/sync/control/synccontroltransfer.cpp
    ${CMAKE_SOURCE_DIR}/src/transfer/impl/sync/interrupt/syncintertransfer.h
    ${CMAKE_SOURCE_DIR}/src/transfer/impl/sync/interrupt/syncintertransfer.cpp

    ${CMAKE_SOURCE_DIR}/src/iocommand/iocommand.h
    ${CMAKE_SOURCE_DIR}/src/iocommand/iocommand.cpp

    ${CMAKE_SOURCE_DIR}/src/usbdevice/usbdevice.h
    ${CMAKE_SOURCE_DIR}/src/usbdevice/usbdevice.cpp
)

target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    ${USB_LIB}
)