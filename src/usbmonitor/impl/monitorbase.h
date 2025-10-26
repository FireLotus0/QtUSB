#pragma once

#include "../../../include/QtUsb/datatypes.h"
#include <qobject.h>
#include <qatomic.h>

QT_USB_NAMESPACE_BEGIN

class UsbMonitor;

class MonitorBase : public QObject {
public:
    explicit MonitorBase(UsbMonitor* usbMonitor, QObject *parent = nullptr);
    virtual ~MonitorBase();

    virtual void addMonitorId(UsbId id) = 0;
    virtual void removeMonitorId(UsbId id) = 0;

    virtual void startMonitor();
    virtual void stopMonitor();

public:
    QAtomicInt monitorFlag{0};
    UsbMonitor* usbMonitor{};
};

QT_USB_NAMESPACE_END

