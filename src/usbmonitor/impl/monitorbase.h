#pragma once

#include "src/datatypes.h"
#include <qobject.h>
#include <qset.h>

QT_USB_NAMESPACE_BEGIN

class MonitorBase : public QObject {
Q_OBJECT
public:
    explicit MonitorBase(QObject *parent = nullptr);

    virtual void addMonitor(UsbId id) = 0;
    virtual void removeMonitor(UsbId id) = 0;

signals:
    void deviceAttached(UsbId);
    void deviceDetached(UsbId);

protected:
    QSet<UsbId> monitorIds;
};

QT_USB_NAMESPACE_END

