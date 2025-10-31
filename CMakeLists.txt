cmake_minimum_required(VERSION 3.2...4.0)

project(QtUsb VERSION 1.0)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_SHARED_LIBS "Build shared library (DLL/.so) instead of static" ON)
# 根据选项设置库类型变量
if(BUILD_SHARED_LIBS)
    set(LIB_TYPE SHARED)
else()
    set(LIB_TYPE STATIC)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

if(MSVC)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif ()

find_package(Qt5 COMPONENTS Core REQUIRED)
find_library(USB_LIB NAMES libusb-1.0 usb-1.0 usb)
if (NOT USB_LIB)
    message(FATAL_ERROR "can`t find libusb")
endif ()

add_library(${PROJECT_NAME}  ${LIB_TYPE}
    include/QtUsb/datatypes.h
    include/QtUsb/usbdevice.h
    include/QtUsb/usbdevmanager.h
    include/QtUsb/usb_namespace.h
    include/QtUsb/qtusb_export.h


    src/datatypes/datatypes.cpp
    src/usbdevmanager/usbdevmanager.cpp

    src/usbmonitor/usbmonitor.h
    src/usbmonitor/usbmonitor.cpp
    src/usbmonitor/impl/monitorbase.h
    src/usbmonitor/impl/monitorbase.cpp
    src/usbmonitor/impl/monitorthreaded.h
    src/usbmonitor/impl/monitorthreaded.cpp
    src/usbmonitor/impl/monitorhotplug.h
    src/usbmonitor/impl/monitorhotplug.cpp

    src/descriptor/usbdescriptor.h
    src/descriptor/usbdescriptor.cpp
    src/descriptor/descriptorbase/descriptordata.h
    src/descriptor/descriptorbase/descriptorbase.h
    src/descriptor/descriptorbase/descriptorbase.cpp
    src/descriptor/device/devicedesc.h
    src/descriptor/device/devicedesc.cpp
    src/descriptor/configuration/configdesc.h
    src/descriptor/configuration/configdesc.cpp
    src/descriptor/interface/interfacedesc.h
    src/descriptor/interface/interfacedesc.cpp
    src/descriptor/interface/interfacealter.h
    src/descriptor/interface/interfacealter.cpp
    src/descriptor/endpoint/endpointdesc.h
    src/descriptor/endpoint/endpointdesc.cpp

    src/transfer/transfercontext.h
    src/transfer/transfercontext.cpp
    src/transfer/impl/strategybase.h
    src/transfer/impl/strategybase.cpp
    src/transfer/impl/sync/bulk/syncbulktransfer.h
    src/transfer/impl/sync/bulk/syncbulktransfer.cpp
    src/transfer/impl/sync/control/synccontroltransfer.h
    src/transfer/impl/sync/control/synccontroltransfer.cpp
    src/transfer/impl/sync/interrupt/syncintertransfer.h
    src/transfer/impl/sync/interrupt/syncintertransfer.cpp

    src/iocommand/iocommand.h
    src/iocommand/iocommand.cpp

    src/usbdevice/usbdevice.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    Qt5::Core
    ${USB_LIB}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    EXPORT_NAME ${PROJECT_NAME}
    DEBUG_POSTFIX "d"
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

set(PACKAGE_LIB_INSTALL_DIR "lib")
configure_package_config_file(
    "${CMAKE_CURRENT_LIST_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION lib/cmake/${PROJECT_NAME}
)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION include)

if(BUILD_SHARED_LIBS)
    install(TARGETS ${PROJECT_NAME}
        EXPORT QtUsbTargets
        RUNTIME DESTINATION dll        # Windows DLL
        LIBRARY DESTINATION dll        # Linux/macOS SO
        ARCHIVE DESTINATION dll        # 动态库有时也有 archive 文件
        INCLUDES DESTINATION include
    )
else()
    install(TARGETS ${PROJECT_NAME}
        EXPORT QtUsbTargets
        ARCHIVE DESTINATION lib        # 静态库
        RUNTIME DESTINATION lib        # 防止异常
        LIBRARY DESTINATION lib        # 防止异常
        INCLUDES DESTINATION include
    )
endif()

install(EXPORT QtUsbTargets
    FILE QtUsbTargets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION lib/cmake/${PROJECT_NAME}
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION lib/cmake/${PROJECT_NAME}
)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(${PROJECT_NAME} PRIVATE QTUSB_EXPORTS)
else ()
    target_compile_definitions(${PROJECT_NAME} PRIVATE QTUSB_STATIC)
endif ()

#add_subdirectory(test EXCLUDE_FROM_ALL)
