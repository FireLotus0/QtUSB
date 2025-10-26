#pragma once

#include <qobject.h>

#include "libusb.h"
#include "usb_namespace.h"
#include "datatypes.h"
#include <qmap.h>
#include <qsharedpointer.h>

QT_USB_NAMESPACE_BEGIN

class UsbDevice;
class UsbMonitor;

class UsbDevManager : public QObject {
    Q_OBJECT

public:
    static UsbDevManager &instance();

    void addMonitorId(UsbId id);

    void removeMonitorId(UsbId id);

    QSharedPointer<UsbDevice> getDevice(UsbId id) const;

signals:
    void deviceAttached(UsbId id);
    void deviceDetached(UsbId id);

private slots:
    void onDeviceAttached(UsbId id, LibUsbDevWrap dev);
    void onDeviceDetached(UsbId id);

private:
    explicit UsbDevManager(QObject *parent = 0);

    ~UsbDevManager();

private:
    UsbMonitor *monitor;
    QMap<UsbId, QSharedPointer<UsbDevice>> devices;
};

QT_USB_NAMESPACE_END
