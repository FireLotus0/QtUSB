#pragma once

#include "../../../include/QtUsb/usb_namespace.h"
#include "../../../include/QtUsb/datatypes.h"
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
    QMap<quint8, EndPointData> endpoints;
};

struct ConfigurationData {
    quint8 configurationValue;
    QMap<quint8, InterfaceData> interfaces;
};

struct DescriptorData {
    bool fullDuplexSupported{false};
    QMap<quint8, ConfigurationData> configurations;
};

QT_USB_NAMESPACE_END