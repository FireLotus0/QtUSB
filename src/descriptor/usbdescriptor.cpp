#include "usbdescriptor.h"
#include "src/descriptor/device/devicedesc.h"

QT_USB_NAMESPACE_BEGIN
UsbDescriptor::UsbDescriptor(libusb_device *device, QObject *parent)
{
    descriptor = new DeviceDesc(device, &descriptorData);
    descriptor->resolveInfo();
    configInfo = descriptor->getContent();
    descriptor->releaseChildren();
    delete descriptor;
    descriptor = nullptr;
}

void UsbDescriptor::printInfo() const {
    qInfo().noquote() << configInfo;
}


QT_USB_NAMESPACE_END

