#pragma once

#include "include/QtUsb//usb_namespace.h"
#include "../../../include/QtUsb/libusb.h"
#include "descriptordata.h"
#include <qstring.h>

QT_USB_NAMESPACE_BEGIN
#define DESCRIPTOR_DESCRIPTION(code, description) std::pair<uint8_t, QString>(code, description)

enum class DescriptorType {
    DEVICE_DESCRIPTOR = 0,
    CONFIGURATION_DESCRIPTOR = 2,
    INTERFACE_DESCRIPTOR = 4,
    INTERFACE_ALTERNATIVE = 6,
    ENDPOINT_DESCRIPTOR = 8,
};

class DescriptorBase {
public:
    explicit DescriptorBase(libusb_device *device);

    virtual ~DescriptorBase();

    virtual void resolveInfo() = 0;

    void printInfo() const;

    void releaseChildren();

    QString getContent() const;

protected:
    template<typename...Args>
    QString genContentLine(Args&&...args) {
        return (printPrefix + ... + args) + "\n";
    }

protected:
    QString content{};
    QString printPrefix{};
    libusb_device *device{};
    DescriptorType descriptorType{};
    QVector<DescriptorBase*> children;
};


QT_USB_NAMESPACE_END
