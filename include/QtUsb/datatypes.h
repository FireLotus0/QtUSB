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
    uint8_t configuration = 0xFF;   // 使用的配置ID
    uint8_t interface = 0xFF;       // 接口
    uint8_t pointNumber = 0xFF;     // 端点
    int readCacheSize = 1024;       // 读取缓冲区大小
    int timeout = 2000;             // 读写超时时间, ms
    bool queuedCommands{false};     // USB 2.0半双工传输，读写操作都是配对进行，USB 3.0支持全双工，设置为true，强制进行命令排队，实现命令同步
    /*
     * 在使用单片机USB实现中断传输时，在Windows平台上，libusb返回实际传输（读/写）的
     * 字节数时会多出1字节, Linux平台上正常。抓包分析，发送的数据确实多出一个字节，值为0。
     * 例如：写入64字节数据，但是Windows平台上写入了65字节数据，即使设备最大包大小仅为64字节，此时设置discardBytes=1，
     * 每次分包写入时减去discardBytes，从而避免分包错误。当读取数据时，如果discardBytes > 0，则会将缓冲区大小设置为端点最大包大小sg + discardBytes，
     * 并且拷贝数据忽略后面的discardBytes个字节数据。目前只在单片机中断传输中使用，并且可能跟设备有关！
     * */
    uint8_t discardBytes = 0;
    /**
     * 在使用单片机USB传输时，连续发送命令，单片机可能无法处理。读取和写入的时间间隔虽然可以由外部调用来控制，
     * 但是内部传输数据时如果存在分包，则存在多次命令写入，即使外部只调用一次read或write，因此添加这个字段进行控制。
     * 例如：单片机每次只能写入64字节，并且处理时间需要5ms，则设置cmdInterval = 5，当写入100字节的数据时，先写64字节，sleep(5),
     * 再写36字节...。目前只在中断传输和批量传输中使用，跟设备处理性能有关。
     */
    uint8_t cmdInterval = 0;
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
Q_DECLARE_METATYPE(QT_USB::TransferDirection)
