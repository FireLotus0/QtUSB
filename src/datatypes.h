#pragma once

#include "src/usb_namespace.h"
#include <QtGlobal>

QT_USB_NAMESPACE_BEGIN

struct UsbId {
    quint16 pid;
    quint16 vid;
};

QT_USB_NAMESPACE_END
