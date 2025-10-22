
#include "configdesc.h"
QT_USB_NAMESPACE_BEGIN

QT_USB::ConfigDesc::ConfigDesc(libusb_device *device, DescriptorType descriptorType)
    : DescriptorBase(device, descriptorType)
{
}

ConfigDesc::~ConfigDesc() {
    for (auto child : children) {
        delete child;
    }
}

QT_USB_NAMESPACE_END
