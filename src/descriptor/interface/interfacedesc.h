#pragma once
#include "src/descriptor/descriptorbase/descriptorbase.h"
#include <qvector.h>

QT_USB_NAMESPACE_BEGIN
class InterfaceDesc : public DescriptorBase
{
public:
    InterfaceDesc(libusb_device* device, libusb_interface interface, int index);

    void resolveInfo() override;

private:
    libusb_interface interface;
    int interfaceIndex;
};

QT_USB_NAMESPACE_END
