#pragma once

#include "QtUsb/usb_namespace.h"
#include "QtUsb/datatypes.h"
#include <qmap.h>

QT_USB_NAMESPACE_BEGIN

struct EndPointData {
    quint8 address;
    TransferType transferType;
    TransferDirection direction;
    uint16_t maxPacketSize;

    bool operator==(const EndPointData &other) const {
        return direction == other.direction && maxPacketSize == other.maxPacketSize
               && transferType == other.transferType && address == other.address;
    }
};

struct InterfaceData {
    quint8 interfaceNumber;
    quint8 interfaceClass;
    quint8 interfaceProtocol;
    quint8 interfaceSubClass;
    QMap<quint8, EndPointData> endpoints;
};

struct ConfigurationData {
    quint8 configurationValue;
    QMap<quint8, InterfaceData> interfaces;
};

struct DescriptorData {
    bool fullDuplexSupported{false};
    quint8 deviceClass = 0;
    quint8 deviceSubClass = 0;
    quint8 deviceProtocol = 0;
    QMap<quint8, ConfigurationData> configurations;
};

QT_USB_NAMESPACE_END