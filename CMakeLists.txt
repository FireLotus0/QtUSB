cmake_minimum_required(VERSION 3.2...4.0)

project(QtUsb VERSION 1.0 LANGUAGES CXX)

# 推荐使用GNUInstallDirs自动选择合适的安装路径
include(GNUInstallDirs)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_SHARED_LIBS "Build shared library (DLL/.so) instead of static" ON)

# 收集头文件和源文件（便于维护）
set(QtUsb_HEADERS
    include/QtUsb/datatypes.h
    include/QtUsb/usbdevice.h
    include/QtUsb/usbdevmanager.h
    include/QtUsb/usb_namespace.h
    include/QtUsb/qtusb_export.h
    src/usbmonitor/usbmonitor.h
    src/usbmonitor/impl/monitorbase.h
    src/usbmonitor/impl/monitorthreaded.h
    src/usbmonitor/impl/monitorhotplug.h
    src/descriptor/usbdescriptor.h
    src/descriptor/descriptorbase/descriptordata.h
    src/descriptor/descriptorbase/descriptorbase.h
    src/descriptor/device/devicedesc.h
    src/descriptor/configuration/configdesc.h
    src/descriptor/interface/interfacedesc.h
    src/descriptor/interface/interfacealter.h
    src/descriptor/endpoint/endpointdesc.h
    src/transfer/transfercontext.h
    src/transfer/impl/strategybase.h
    src/transfer/impl/sync/bulk/syncbulktransfer.h
    src/transfer/impl/sync/control/synccontroltransfer.h
    src/transfer/impl/sync/interrupt/syncintertransfer.h
    src/iocommand/iocommand.h
)

set(QtUsb_SOURCES
    src/datatypes/datatypes.cpp
    src/usbdevmanager/usbdevmanager.cpp
    src/usbmonitor/usbmonitor.cpp
    src/usbmonitor/impl/monitorbase.cpp
    src/usbmonitor/impl/monitorthreaded.cpp
    src/usbmonitor/impl/monitorhotplug.cpp
    src/descriptor/usbdescriptor.cpp
    src/descriptor/descriptorbase/descriptorbase.cpp
    src/descriptor/device/devicedesc.cpp
    src/descriptor/configuration/configdesc.cpp
    src/descriptor/interface/interfacedesc.cpp
    src/descriptor/interface/interfacealter.cpp
    src/descriptor/endpoint/endpointdesc.cpp
    src/transfer/transfercontext.cpp
    src/transfer/impl/strategybase.cpp
    src/transfer/impl/sync/bulk/syncbulktransfer.cpp
    src/transfer/impl/sync/control/synccontroltransfer.cpp
    src/transfer/impl/sync/interrupt/syncintertransfer.cpp
    src/iocommand/iocommand.cpp
    src/usbdevice/usbdevice.cpp
)

# MSVC 编译选项（target级别，归规范写法）
if(MSVC)
    set(MSVC_UTF8_FLAGS "/utf-8")
endif()

find_package(Qt5 REQUIRED COMPONENTS Core)
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
    Qt5::Core
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

 add_subdirectory(test EXCLUDE_FROM_ALL)
