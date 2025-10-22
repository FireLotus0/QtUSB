#pragma once
#include "src/descriptor/descriptorbase.h"
#include <qvector.h>

QT_USB_NAMESPACE_BEGIN
class ConfigDesc : public DescriptorBase
{
public:
    ConfigDesc(libusb_device* device, DescriptorType descriptorType);
    ~ConfigDesc();


private:
    libusb_config_descriptor config;
    QVector<DescriptorBase*> children;
};

QT_USB_NAMESPACE_END
