#include "datatypes.h"
#include <qdebug.h>

QT_USB_NAMESPACE_BEGIN

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


QT_USB_NAMESPACE_END

QDebug &QT_USB::operator<<(QDebug &out, const QT_USB::UsbId &data) {
    out << "USB ID: pid=" << data.pid <<", vid=" << data.vid;
    return out;
}
