#pragma once

#include "../../include/QtUsb/datatypes.h"
#include "include/QtUsb/libusb.h"
#include <qobject.h>

QT_USB_NAMESPACE_BEGIN

class MonitorBase;

class UsbMonitor : public QObject {
Q_OBJECT
public:
    explicit UsbMonitor(QObject *parent = nullptr);
    ~UsbMonitor();

    void addMonitorId(UsbId id);

    void removeMonitorId(UsbId id);

    void start() const;

    void stop() const;

signals:
    void deviceAttached(UsbId id, LibUsbDevWrap device);
    void deviceDetached(UsbId id);

private:
    void initMonitor();
private:
    MonitorBase*  monitor;
};


QT_USB_NAMESPACE_END
