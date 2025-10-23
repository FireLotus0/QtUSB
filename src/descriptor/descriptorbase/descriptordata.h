#pragma once

#include "src/usb_namespace.h"
#include <qmap.h>

QT_USB_NAMESPACE_BEGIN

enum class TransferType {
    CONTROL = 0,
    SYNC,
    BULK,
    INTERRUPT,
};

QString transferTypeToString(TransferType t);

enum class TransferDirection {
    HOST_TO_DEVICE = 0,
    DEVICE_TO_HOST,
};
QString transferDirectionToString(TransferDirection d);

struct EndPointData {
    quint8 address;
    TransferType transferType;
    TransferDirection direction;
    uint16_t maxPacketSize;

    bool operator==(const EndPointData &other) const;
};

struct InterfaceData {
    quint8 interfaceNumber;
    QMap<quint8, EndPointData> endpoints;
};

struct ConfigurationData {
    quint8 configurationValue;
    QMap<quint8, InterfaceData> interfaces;
};

struct DescriptorData {
    QMap<quint8, ConfigurationData> configurations;
};

struct ActiveUSBConfig {
    quint8 configuration = 0xFF;
    quint8 interface = 0xFF;
};

QT_USB_NAMESPACE_END