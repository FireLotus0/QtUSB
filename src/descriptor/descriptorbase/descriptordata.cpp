#include "descriptordata.h"

QT_USB_NAMESPACE_BEGIN
QString transferTypeToString(TransferType t) {
    switch (t) {
        case TransferType::CONTROL: return "Control Transfer";
        case TransferType::SYNC: return "Sync Transfer";
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

bool EndPointData::operator==(const EndPointData &other) const {
    return direction == other.direction && maxPacketSize == other.maxPacketSize
        && transferType == other.transferType && address == other.address;
}

QT_USB_NAMESPACE_END
