#pragma once

#include "QtUsb/libusb.h"
#include "QtUsb/usb_namespace.h"
#include "descriptor/descriptorbase/descriptordata.h"
#include <qobject.h>
#include <qmap.h>

QT_USB_NAMESPACE_BEGIN

class DescriptorBase;

class UsbDescriptor : public QObject {
public:
    Q_DECLARE_FLAGS(DeviceTypes, DeviceType)

public:
    UsbDescriptor() = default;

    explicit UsbDescriptor(libusb_device* device, QObject *parent = nullptr);
    ~UsbDescriptor() = default;

    UsbDescriptor(const UsbDescriptor& other);
    UsbDescriptor& operator=(const UsbDescriptor& other);

    void printInfo() const;

    DescriptorData getDescriptorData() const;

    inline static QMap<UsbId, UsbDescriptor> descriptors;

    DeviceTypes getDeviceTypes() const;
private:
    void resolveDeviceType();

    DeviceType classCode2Type(libusb_class_code code) const;

private:
    DescriptorBase* descriptor{};
    DeviceTypes deviceType{DeviceType::UNDEFINED};
    QString configInfo;
    DescriptorData descriptorData;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(UsbDescriptor::DeviceTypes)

QT_USB_NAMESPACE_END
