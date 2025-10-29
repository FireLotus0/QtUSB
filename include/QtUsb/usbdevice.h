#pragma once

#include "QtUsb/usb_namespace.h"
#include "QtUsb/datatypes.h"
#include <qobject.h>

QT_USB_NAMESPACE_BEGIN
class IoCommand;
class UsbDescriptor;

class UsbDevice : public QObject {
    Q_OBJECT

public:
    explicit UsbDevice(QObject *parent = nullptr);

    ~UsbDevice();

    void setValid(bool valid);

    void openDevice(UsbId usbId, libusb_device *device);

    void setConfiguration(ActiveUSBConfig newCfg);

    void read() const;

    void write(QByteArray &&data) const;

    void setSpeedPrintEnable(bool enable);

signals:
    void readFinished(const QByteArray &data);
    void writeFinished();
    void errorOccurred(int errorCode, const QString& errorString);

private:
    std::atomic<bool> validFlag{false};
    UsbId id;
    UsbDescriptor* descriptor;
    ActiveUSBConfig usbCfg;
    IoCommand *ioCommand;
    libusb_device_handle *handle{nullptr};
};

QT_USB_NAMESPACE_END
