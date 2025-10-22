#pragma once

#include "src/datatypes.h"
#include <qobject.h>

QT_USB_NAMESPACE_BEGIN

class MonitorBase;

class UsbMonitor : public QObject {
Q_OBJECT
public:
    static UsbMonitor& instance();

    void addMonitorId(UsbId id);

    void removeMonitorId(UsbId id);

    void start();

    void stop();

signals:
    void deviceAttached(UsbId id);
    void deviceDetached(UsbId id);

private:
    explicit UsbMonitor(QObject *parent = nullptr);

    void initMonitor();
private:
    MonitorBase*  monitor;
};


QT_USB_NAMESPACE_END