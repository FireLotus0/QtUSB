#include "usbdescriptor.h"
#include "src/descriptor/device/devicedesc.h"

QT_USB_NAMESPACE_BEGIN
UsbDescriptor::UsbDescriptor(libusb_device *device, QObject *parent)
{
    descriptor = new DeviceDesc(device, &descriptorData);
    descriptor->resolveInfo();
    qDebug() << "configurations count " <<descriptorData.configurations.size();
    for (auto cfg : descriptorData.configurations) {
        qDebug() << "interface count: " << cfg.interfaces.size();
        for (auto e : cfg.interfaces) {
            qDebug() << "endpoint count: " << e.endpoints.size();
        }
    }
    configInfo = descriptor->getContent();
    descriptor->releaseChildren();
    delete descriptor;
    descriptor = nullptr;
}

void UsbDescriptor::printInfo() const {
    qInfo().noquote() << configInfo;
}

DescriptorData UsbDescriptor::getDescriptorData() const {
    return descriptorData;
}


QT_USB_NAMESPACE_END

