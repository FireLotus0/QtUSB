#pragma once

#include "src/usb_namespace.h"
#include "src/iocommand/iocommand.h"
#include <qobject.h>

QT_USB_NAMESPACE_BEGIN

class UsbDevice : public QObject {
Q_OBJECT
public:
    explicit UsbDevice(UsbId usbId, libusb_device* device, QObject *parent = nullptr);

    ~UsbDevice();

private:
    UsbId id;
    ActiveUSBConfig usbCfg;
    IoCommand* ioCommand;
    libusb_device_handle* handle{nullptr};
};

QT_USB_NAMESPACE_END