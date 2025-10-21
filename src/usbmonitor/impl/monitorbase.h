#pragma once

#include "src/datatypes.h"
#include <qobject.h>
#include <qatomic.h>

QT_USB_NAMESPACE_BEGIN

class MonitorBase : public QObject {
public:
    explicit MonitorBase(QObject *parent = nullptr);
    virtual ~MonitorBase();

    virtual void addMonitorId(UsbId id) = 0;
    virtual void removeMonitorId(UsbId id) = 0;

    virtual void startMonitor();
    virtual void stopMonitor();

protected:
    QAtomicInt monitorFlag{0};
};

QT_USB_NAMESPACE_END

