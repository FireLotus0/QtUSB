#include "usbdevice.h"

QT_USB_NAMESPACE_BEGIN

UsbDevice::UsbDevice(UsbId usbId, libusb_device *device, QObject *parent)
    : id(usbId)
    , QObject(parent)
{

}


QT_USB::UsbDevice::~UsbDevice() {
    delete ioCommand;
}

QT_USB_NAMESPACE_END