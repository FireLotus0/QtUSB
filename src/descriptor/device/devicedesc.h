#pragma once
#include "src/descriptor/descriptorbase/descriptorbase.h"
#include <qvector.h>
#include <qmap.h>

QT_USB_NAMESPACE_BEGIN
class DeviceDesc : public DescriptorBase
{
public:
    explicit DeviceDesc(libusb_device* device);
    ~DeviceDesc()  = default;

    void resolveInfo() override;

private:
    static QMap<uint8_t, QString> devClassInfo;
    static QMap<uint8_t, QString> devSpeedInfo;
private:
    libusb_device_descriptor desc;
};

QT_USB_NAMESPACE_END
