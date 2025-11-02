#pragma once

#include "descriptor/descriptorbase/descriptorbase.h"
#include <qvector.h>

QT_USB_NAMESPACE_BEGIN
class InterfaceAlter : public DescriptorBase {
public:
    InterfaceAlter(libusb_device *device, libusb_interface_descriptor interfaceDesc, ConfigurationData *configurationData);

    void resolveInfo() override;

private:
    libusb_interface_descriptor desc;
    ConfigurationData *configurationData;
};

QT_USB_NAMESPACE_END
