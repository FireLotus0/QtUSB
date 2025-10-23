#pragma once

#include "src/libusb.h"
#include  "src/usb_namespace.h"
#include <qobject.h>

QT_USB_NAMESPACE_BEGIN

class DescriptorBase;

class UsbDescriptor : public QObject {
public:
    explicit UsbDescriptor(libusb_device* device, QObject *parent = nullptr);

    ~UsbDescriptor();

    void printInfo();

private:
    DescriptorBase* descriptor{};
};

QT_USB_NAMESPACE_END