#include "../include/QtUsb/usb_namespace.h"
#include "../include/QtUsb/usbdevmanager.h"
#include "../include/QtUsb/usbdevice.h"
#include "../../include/QtUsb/datatypes.h"
#include <qloggingcategory.h>

QT_USB_NAMESPACE_BEGIN

Q_LOGGING_CATEGORY(usbCategory, "usb.category");

bool QT_USB::UsbId::operator==(const QT_USB::UsbId &other) const {
    return pid == other.pid && vid == other.vid;
}

bool QT_USB::UsbId::operator<(const QT_USB::UsbId &other) const {
    if(vid < other.vid) {
        return true;
    }
    if(vid > other.vid) {
        return false;
    }
    return pid < other.pid;
}

QString transferTypeToString(TransferType t) {
    switch (t) {
        case TransferType::CONTROL: return "Control Transfer";
        case TransferType::ISO: return "ISO Transfer";
        case TransferType::BULK: return "Bulk Transfer";
        case TransferType::INTERRUPT: return "Interrupt Transfer";
    }
    return "Unknown Transfer Type";
}

QString transferDirectionToString(TransferDirection d) {
    switch (d) {
        case TransferDirection::DEVICE_TO_HOST: return "Device -> Host";
        case TransferDirection::HOST_TO_DEVICE: return "Host -> Device";
    }
    return "Unknown Transfer Direction";
}

TransferStrategy transTypeToStrategy(bool sync, TransferType type) {
    if(sync) {
        switch(type) {
            case TransferType::BULK: return TransferStrategy::SYNC_BULK;
            case TransferType::INTERRUPT: return TransferStrategy::SYNC_INTERRUPT;
            case TransferType::CONTROL: return TransferStrategy::SYNC_CONTROL;
            default: assert(false);
        }
    } else {
        assert(false);
    }
    return TransferStrategy::SYNC_BULK;
}

IoData::IoData(QT_USB::IoData &&other) noexcept {
    data = std::move(other.data);
    address = other.address;
    maxPacketSize = other.maxPacketSize;
    resultCode = other.maxPacketSize;
    transferDirection = other.transferDirection;
    transferStrategy = other.transferStrategy;
    handle = other.handle;
    controlRequestData = std::move(other.controlRequestData);
}

IoData::IoData(const IoData &other) {
    data = other.data;
    address = other.address;
    maxPacketSize = other.maxPacketSize;
    resultCode = other.resultCode;
    transferDirection = other.transferDirection;
    transferStrategy = other.transferStrategy;
    handle = other.handle;
    controlRequestData = other.controlRequestData;
}

IoData &IoData::operator=(const IoData &other) {
    data = other.data;
    address = other.address;
    maxPacketSize = other.maxPacketSize;
    resultCode = other.resultCode;
    transferDirection = other.transferDirection;
    transferStrategy = other.transferStrategy;
    handle = other.handle;
    controlRequestData = other.controlRequestData;
    return *this;
}

IoData &IoData::operator=(IoData &&other) {
    data = std::move(other.data);
    address = other.address;
    maxPacketSize = other.maxPacketSize;
    resultCode = other.maxPacketSize;
    transferDirection = other.transferDirection;
    transferStrategy = other.transferStrategy;
    handle = other.handle;
    controlRequestData = std::move(other.controlRequestData);\
    return *this;
}

QT_USB_NAMESPACE_END

QDebug &QT_USB::operator<<(QDebug &out, const QT_USB::UsbId &data) {
    out << "USB ID: pid=" << data.pid << "(hex:" << QString::number(data.pid, 16) << ")" <<", vid=" << data.vid << "(hex:" << QString::number(data.vid, 16)
        << ")";
    return out;
}
