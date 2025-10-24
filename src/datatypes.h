#pragma once

#include <qmetatype.h>

#include "src/usb_namespace.h"
#include "libusb.h"
#include <QtGlobal>

QT_USB_NAMESPACE_BEGIN

struct UsbId {
    quint16 pid;
    quint16 vid;

    bool operator==(const UsbId &other) const;

    bool operator<(const UsbId& other) const;

    friend QDebug& operator<<(QDebug& out, const UsbId& data);
};

struct LibUsbDevWrap {
    libusb_device* device;
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

struct ControlRequestData {
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
    IoData(IoData&& data);

    QByteArray data;
    uint8_t address{0};
    uint16_t maxPacketSize{0};
    int resultCode{LIBUSB_SUCCESS};
    TransferDirection transferDirection;
    TransferStrategy transferStrategy;
    libusb_device_handle* handle{nullptr};
    ControlRequestData controlRequestData;
};



QT_USB_NAMESPACE_END

Q_DECLARE_METATYPE(QT_USB::UsbId)
Q_DECLARE_METATYPE(QT_USB::LibUsbDevWrap)
Q_DECLARE_METATYPE(QT_USB::IoData)
