#pragma once

#include "QtUsb/datatypes.h"
#include "QtUsb/libusb.h"
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

    void addMonitorClass(uint8_t devClass);

    void removeMonitorClass(uint8_t devClass);

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
