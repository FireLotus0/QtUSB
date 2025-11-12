#pragma once

#include <qmetatype.h>

#include "usb_namespace.h"
#include "QtUsb/libusb.h"
#include "QtUsb/qtusb_export.h"
#include <qloggingcategory.h>
#include <QtGlobal>

QT_USB_NAMESPACE_BEGIN

Q_DECLARE_LOGGING_CATEGORY(usbCategory)

struct QTUSB_API UsbId {
    quint16 pid;
    quint16 vid;

    bool operator==(const UsbId &other) const;

    bool operator<(const UsbId& other) const;

    friend QDebug& operator<<(QDebug& out, const UsbId& data);
};

enum class TransferType {
    CONTROL = 0,
    ISO,
    BULK,
    INTERRUPT,
};

QString transferTypeToString(TransferType t);

enum class TransferDirection {
    HOST_TO_DEVICE = 0,
    DEVICE_TO_HOST,
};
QString transferDirectionToString(TransferDirection d);

enum class TransferStrategy {
    SYNC_CONTROL,
    SYNC_BULK,
    SYNC_INTERRUPT,
    ASYNC_CONTROL,
    ASYNC_ISO,
    ASYNC_BULK,
    ASYNC_INTERRUPT
};

TransferStrategy transTypeToStrategy(bool sync, TransferType type);

struct QTUSB_API ControlRequestData {
    /*
     * bit7: 方向 LIBUSB_ENDPOINT_IN, LIBUSB_ENDPOINT_OUT
     * bit[5:6]: 类型 LIBUSB_REQUEST_TYPE_STANDARD, LIBUSB_REQUEST_TYPE_CLASS, LIBUSB_REQUEST_TYPE_VENDOR, LIBUSB_REQUEST_TYPE_RESERVED
     * bit[0:4]: 目标 LIBUSB_RECIPIENT_DEVICE, LIBUSB_RECIPIENT_INTERFACE, LIBUSB_RECIPIENT_ENDPOINT, LIBUSB_RECIPIENT_OTHER
     */
    uint8_t requestType;
    // 请求码
    uint8_t request;
    // 请求的参数值，含义依赖于 request
    uint8_t value;
    // 请求的索引参数，通常用于指定接口、端点等对象
    uint16_t index;
    // 本次控制传输要求发送或接收的数据长度
    uint16_t length;
};

struct IoData {
    IoData() = default;
    IoData(const IoData& data);
    IoData(IoData&& data) noexcept;

    IoData& operator=(const IoData& data);
    IoData& operator=(IoData&& data);

    QByteArray data;
    uint8_t address{0};
    uint16_t maxPacketSize{0};
    int resultCode{LIBUSB_SUCCESS};
    TransferDirection transferDirection;
    TransferStrategy transferStrategy;
    libusb_device_handle* handle{nullptr};
    ControlRequestData controlRequestData;
};

struct QTUSB_API ActiveUSBConfig {
    quint8 configuration = 0xFF;    // 使用的配置ID
    quint8 interface = 0xFF;        // 接口
    uint8_t pointNumber = 0xFF;     // 端点
    int readCacheSize = 1024;       // 读取缓冲区大小
    bool queuedCommands{false};     // USB 2.0半双工传输，读写操作都是配对进行，USB 3.0支持全双工，设置为true，强制进行命令排队，实现命令同步
};

enum class DeviceType {
    ANY_CLASS = 0,
    UNDEFINED_CLASS = 1,
    USB_CLASS_AUDIO = 1 << 1,
    USB_CLASS_COMM = 1 << 2,
    USB_CLASS_HID = 1 << 3,
    USB_CLASS_PHYSICAL = 1 << 4,
    USB_CLASS_IMAGE = 1 << 5,
    USB_CLASS_PRINTER = 1 << 6,
    USB_CLASS_MASS_STORAGE = 1 << 7,
    USB_CLASS_HUB = 1 << 8,
    USB_CLASS_DATA = 1 << 9,
    USB_CLASS_SMART_CARD = 1 << 10,
    USB_CLASS_CONTENT_SECURITY = 1 << 11,
    USB_CLASS_VIDEO = 1 << 12,
    USB_CLASS_PERSONAL_HEALTHCARE = 1 << 13,
    USB_CLASS_DIAGNOSTIC_DEVICE = 1 << 14,
    USB_CLASS_WIRELESS = 1 << 15,
    USB_CLASS_MISCELLANEOUS = 1 << 16,
    USB_CLASS_APPLICATION = 1 << 17,
    USB_CLASS_VENDOR_SPEC = 1 << 18,
};

QT_USB_NAMESPACE_END

Q_DECLARE_METATYPE(QT_USB::UsbId)
Q_DECLARE_METATYPE(QT_USB::IoData)
Q_DECLARE_METATYPE(QT_USB::DeviceType)
