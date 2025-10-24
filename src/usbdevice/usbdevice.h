#pragma once

#include "src/usb_namespace.h"
#include "src/iocommand/iocommand.h"
#include <qobject.h>

#include "src/descriptor/usbdescriptor.h"

QT_USB_NAMESPACE_BEGIN
class UsbDevice : public QObject {
    Q_OBJECT

public:
    explicit UsbDevice(QObject *parent = nullptr);

    ~UsbDevice();

    void openDevice(UsbId usbId, libusb_device *device);

    void setConfiguration(ActiveUSBConfig newCfg);

    void read() const;

    void write(QByteArray &&data) const;

signals:
    void deviceConnected(QT_USB::UsbId id);
    void deviceDisconnected(QT_USB::UsbId id);
    void readFinished(const QByteArray &data);
    void errorOccurred(int errorCode, const QString& errorString);


private:
    UsbId id;
    UsbDescriptor* descriptor;
    ActiveUSBConfig usbCfg;
    IoCommand *ioCommand;
    libusb_device_handle *handle{nullptr};
};

QT_USB_NAMESPACE_END
