#include "descriptorbase.h"

QT_USB_NAMESPACE_BEGIN
DescriptorBase::DescriptorBase(libusb_device *device, DescriptorType descriptorType)
    : device(device), descriptorType(descriptorType)
{
}

QString DescriptorBase::getPrintPrefix() const {
    return QString(static_cast<int>(descriptorType), ' ');
}

QT_USB_NAMESPACE_END
