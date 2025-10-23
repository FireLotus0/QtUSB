#pragma once

#include "src/descriptor/descriptorbase/descriptorbase.h"
#include <qvector.h>

QT_USB_NAMESPACE_BEGIN
class InterfaceAlter : public DescriptorBase
{
public:
    InterfaceAlter(libusb_device* device, libusb_interface_descriptor interfaceDesc);

    void resolveInfo() override;

private:
    libusb_interface_descriptor desc;
};

QT_USB_NAMESPACE_END

