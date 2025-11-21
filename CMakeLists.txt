cmake_minimum_required(VERSION 3.2...4.0)

project(QtUsb VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(GNUInstallDirs)

option(BUILD_EXAMPLES "Build examples" ON)
option(BUILD_SHARED_LIBS "Build shared library (DLL/.so) instead of static" OFF)

# 收集头文件和源文件（便于维护）
set(QtUsb_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/include/QtUsb/datatypes.h
    ${CMAKE_CURRENT_LIST_DIR}/include/QtUsb/usbdevice.h
    ${CMAKE_CURRENT_LIST_DIR}/include/QtUsb/usbdevmanager.h
    ${CMAKE_CURRENT_LIST_DIR}/include/QtUsb/usb_namespace.h
    ${CMAKE_CURRENT_LIST_DIR}/include/QtUsb/qtusb_export.h
    ${CMAKE_CURRENT_LIST_DIR}/src/usbmonitor/usbmonitor.h
    ${CMAKE_CURRENT_LIST_DIR}/src/usbmonitor/impl/monitorbase.h
    ${CMAKE_CURRENT_LIST_DIR}/src/usbmonitor/impl/monitorthreaded.h
    ${CMAKE_CURRENT_LIST_DIR}/src/usbmonitor/impl/monitorhotplug.h
    ${CMAKE_CURRENT_LIST_DIR}/src/descriptor/usbdescriptor.h
    ${CMAKE_CURRENT_LIST_DIR}/src/descriptor/descriptorbase/descriptordata.h
    ${CMAKE_CURRENT_LIST_DIR}/src/descriptor/descriptorbase/descriptorbase.h
    ${CMAKE_CURRENT_LIST_DIR}/src/descriptor/device/devicedesc.h
    ${CMAKE_CURRENT_LIST_DIR}/src/descriptor/configuration/configdesc.h
    ${CMAKE_CURRENT_LIST_DIR}/src/descriptor/interface/interfacedesc.h
    ${CMAKE_CURRENT_LIST_DIR}/src/descriptor/interface/interfacealter.h
    ${CMAKE_CURRENT_LIST_DIR}/src/descriptor/endpoint/endpointdesc.h
    ${CMAKE_CURRENT_LIST_DIR}/src/transfer/transfercontext.h
    ${CMAKE_CURRENT_LIST_DIR}/src/transfer/impl/strategybase.h
    ${CMAKE_CURRENT_LIST_DIR}/src/transfer/impl/sync/bulk/syncbulktransfer.h
    ${CMAKE_CURRENT_LIST_DIR}/src/transfer/impl/sync/control/synccontroltransfer.h
    ${CMAKE_CURRENT_LIST_DIR}/src/transfer/impl/sync/interrupt/syncintertransfer.h
    ${CMAKE_CURRENT_LIST_DIR}/src/iocommand/iocommand.h
)

set(QtUsb_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/src/datatypes/datatypes.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/usbdevmanager/usbdevmanager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/usbmonitor/usbmonitor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/usbmonitor/impl/monitorbase.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/usbmonitor/impl/monitorthreaded.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/usbmonitor/impl/monitorhotplug.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/descriptor/usbdescriptor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/descriptor/descriptorbase/descriptorbase.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/descriptor/device/devicedesc.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/descriptor/configuration/configdesc.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/descriptor/interface/interfacedesc.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/descriptor/interface/interfacealter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/descriptor/endpoint/endpointdesc.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/transfer/transfercontext.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/transfer/impl/strategybase.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/transfer/impl/sync/bulk/syncbulktransfer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/transfer/impl/sync/control/synccontroltransfer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/transfer/impl/sync/interrupt/syncintertransfer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/iocommand/iocommand.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/usbdevice/usbdevice.cpp
)

# MSVC 编译选项（target级别，归规范写法）
if(MSVC)
    set(MSVC_UTF8_FLAGS "/utf-8")
endif()

find_package(Qt6 COMPONENTS Core QUIET)
if(Qt6_FOUND)
    set(QT_CORE_LIB Qt6::Core)
    message(STATUS "Using Qt6")
elseif(NOT Qt6_FOUND)
    find_package(Qt5 COMPONENTS Core REQUIRED)
    message(STATUS "Using Qt5")
    set(QT_CORE_LIB Qt5::Core)
endif()

find_library(USB_LIB NAMES libusb-1.0 usb-1.0 usb)
if(NOT USB_LIB)
    message(FATAL_ERROR "Cannot find libusb")
endif()

add_library(QtUsb ${QtUsb_HEADERS} ${QtUsb_SOURCES})

target_include_directories(QtUsb
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>  # 添加 src 目录
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(QtUsb
    PUBLIC
    ${QT_CORE_LIB}
    ${USB_LIB}
)

# MSVC特有编译选项
if(MSVC)
    target_compile_options(QtUsb PRIVATE ${MSVC_UTF8_FLAGS})
endif()

set_target_properties(QtUsb PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    DEBUG_POSTFIX "d"
    EXPORT_NAME QtUsb
)

# 导出宏定义（静态/动态）
if(BUILD_SHARED_LIBS)
    target_compile_definitions(QtUsb PRIVATE QTUSB_EXPORTS)
else()
    target_compile_definitions(QtUsb PRIVATE QTUSB_STATIC)
endif()

# Package config
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/QtUsbConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_LIST_DIR}/cmake/QtUsbConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/QtUsbConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QtUsb
)

# 安装头文件
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# 安装库（归规范路径）
install(TARGETS QtUsb
    EXPORT QtUsbTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT QtUsbTargets
    FILE QtUsbTargets.cmake
    NAMESPACE QtUsb::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QtUsb
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/QtUsbConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/QtUsbConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QtUsb
)

if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif ()