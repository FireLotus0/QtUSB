#include "usbdescriptor.h"
#include "src/descriptor/device/devicedesc.h"

QT_USB_NAMESPACE_BEGIN
UsbDescriptor::UsbDescriptor(libusb_device *device, QObject *parent)
{
    descriptor = new DeviceDesc(device);
    descriptor->resolveInfo();
}

UsbDescriptor::~UsbDescriptor() {
    delete descriptor;
}

void UsbDescriptor::printInfo() {
    if(descriptor) {
        descriptor->printInfo();
    } else {
        qWarning() << "Descriptor is not created yet!";
    }
}


QT_USB_NAMESPACE_END

