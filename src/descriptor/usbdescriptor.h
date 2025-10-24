#pragma once

#include "src/libusb.h"
#include  "src/usb_namespace.h"
#include "src/descriptor/descriptorbase/descriptordata.h"
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