#pragma once

#include <qmetatype.h>

#include "src/usb_namespace.h"
#include <QtGlobal>

QT_USB_NAMESPACE_BEGIN

struct UsbId {
    quint16 pid;
    quint16 vid;

    bool operator==(const UsbId &other) const;

    bool operator<(const UsbId& other) const;

    friend QDebug& operator<<(QDebug& out, const UsbId& data);
};

QT_USB_NAMESPACE_END

Q_DECLARE_METATYPE(QT_USB::UsbId)
