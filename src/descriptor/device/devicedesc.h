#pragma once
#include "descriptor/descriptorbase/descriptorbase.h"
#include <qmap.h>

QT_USB_NAMESPACE_BEGIN
class DeviceDesc : public DescriptorBase
{
public:
    explicit DeviceDesc(libusb_device* device, DescriptorData* descriptorData);
    ~DeviceDesc()  = default;

    void resolveInfo() override;

private:
    static QMap<uint8_t, QString> devClassInfo;
    static QMap<uint8_t, QString> devSpeedInfo;
private:
    libusb_device_descriptor desc;
    DescriptorData* descriptorData;
};

QT_USB_NAMESPACE_END
