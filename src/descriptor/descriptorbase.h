#pragma once

#include "src/usb_namespace.h"
#include "src/libusb.h"
#include <qstring.h>
#include <qdebug.h>

QT_USB_NAMESPACE_BEGIN
enum class DescriptorType {
    DEVICE_DESCRIPTOR = 1,
    CONFIGURATION_DESCRIPTOR,
    INTERFACE_DESCRIPTOR,
    ENDPOINT_DESCRIPTOR,
};

class DescriptorBase {
public:
    DescriptorBase(libusb_device *device, DescriptorType descriptorType);

    virtual ~DescriptorBase() = default;

protected:
    virtual QString toString() const = 0;

    virtual void parseDescriptor() = 0;

protected:
    QString getPrintPrefix() const;

protected:
    libusb_device *device{};
    DescriptorType descriptorType;
};


QT_USB_NAMESPACE_END
