#pragma once
#include "src/descriptor/descriptorbase/descriptorbase.h"
#include <qvector.h>

QT_USB_NAMESPACE_BEGIN
class ConfigDesc : public DescriptorBase {
public:
    ConfigDesc(libusb_device *device, int index, DescriptorData *descriptorData);

    void resolveInfo() override;

private:
    libusb_config_descriptor *desc;
    DescriptorData *descriptorData;
    int configIndex;
};

QT_USB_NAMESPACE_END
