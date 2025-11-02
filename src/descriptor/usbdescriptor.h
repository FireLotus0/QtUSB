#pragma once

#include "QtUsb/libusb.h"
#include "QtUsb/usb_namespace.h"
#include "descriptor/descriptorbase/descriptordata.h"
#include <qobject.h>

QT_USB_NAMESPACE_BEGIN

class DescriptorBase;

class UsbDescriptor : public QObject {
public:
    explicit UsbDescriptor(libusb_device* device, QObject *parent = nullptr);

    ~UsbDescriptor() = default;

    void printInfo() const;

    DescriptorData getDescriptorData() const;

private:
    DescriptorBase* descriptor{};
    QString configInfo;
    DescriptorData descriptorData;
};

QT_USB_NAMESPACE_END