#pragma once

#include "src/usb_namespace.h"
#include "src/libusb.h"
#include <qstring.h>
#include <qdebug.h>

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

    virtual void resolveInfo();

    void printInfo() const;

protected:
    template<typename...Args>
    QString genContentLine(Args&&...args) {
        return (printPrefix + ... + args) + "\n";
    }

    QString getContent() const;

protected:
    QString content{};
    QString printPrefix{};
    libusb_device *device{};
    DescriptorType descriptorType{};
    QVector<DescriptorBase*> children;
};


QT_USB_NAMESPACE_END
