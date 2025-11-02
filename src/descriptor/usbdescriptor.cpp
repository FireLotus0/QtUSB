#include "usbdescriptor.h"
#include "descriptor/device/devicedesc.h"
#include <qloggingcategory.h>

QT_USB_NAMESPACE_BEGIN

const QLoggingCategory &usbCategory();

UsbDescriptor::UsbDescriptor(libusb_device *device, QObject *parent)
{
    descriptor = new DeviceDesc(device, &descriptorData);
    descriptor->resolveInfo();
    qCInfo(usbCategory) << "configurations count " <<descriptorData.configurations.size();
    for (auto cfg : descriptorData.configurations) {
        qCInfo(usbCategory) << "interface count: " << cfg.interfaces.size();
        for (auto e : cfg.interfaces) {
            qCInfo(usbCategory) << "endpoint count: " << e.endpoints.size();
        }
    }
    configInfo = descriptor->getContent();
    descriptor->releaseChildren();
    delete descriptor;
    descriptor = nullptr;
    libusb_unref_device(device);
}

void UsbDescriptor::printInfo() const {
    qCInfo(usbCategory).noquote() << configInfo;
}

DescriptorData UsbDescriptor::getDescriptorData() const {
    return descriptorData;
}


QT_USB_NAMESPACE_END

