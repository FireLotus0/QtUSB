#include "usbdescriptor.h"
#include "descriptor/device/devicedesc.h"
#include <qloggingcategory.h>

QT_USB_NAMESPACE_BEGIN
const QLoggingCategory &usbCategory();

UsbDescriptor::UsbDescriptor(libusb_device *device, QObject *parent) {
    descriptor = new DeviceDesc(device, &descriptorData);
    descriptor->resolveInfo();
    resolveDeviceType();
    configInfo = descriptor->getContent();
    descriptor->releaseChildren();
    delete descriptor;
    descriptor = nullptr;
}

UsbDescriptor::UsbDescriptor(const UsbDescriptor &other) {
    deviceType = other.deviceType;
    configInfo = other.configInfo;
    descriptorData = other.descriptorData;
}

UsbDescriptor & UsbDescriptor::operator=(const UsbDescriptor &other) {
    deviceType = other.deviceType;
    configInfo = other.configInfo;
    descriptorData = other.descriptorData;
    return *this;
}

void UsbDescriptor::printInfo() const {
    qCInfo(usbCategory).noquote() << configInfo;
}

DescriptorData UsbDescriptor::getDescriptorData() const {
    return descriptorData;
}

UsbDescriptor::DeviceTypes UsbDescriptor::getDeviceTypes() const {
    return deviceType;
}

void UsbDescriptor::resolveDeviceType() {
    auto type = classCode2Type(static_cast<libusb_class_code>(descriptorData.deviceClass));
    if (type == DeviceType::UNDEFINED_CLASS) {
        for (const auto& cfg : descriptorData.configurations) {
            for (const auto& interface : cfg.interfaces) {
                deviceType |= classCode2Type(static_cast<libusb_class_code>(interface.interfaceClass));
            }
        }
    }
}

DeviceType UsbDescriptor::classCode2Type(libusb_class_code code) const {
    if (code == LIBUSB_CLASS_IMAGE) {
        return DeviceType::USB_CLASS_IMAGE;
    }
    switch (code) {
        case LIBUSB_CLASS_PER_INTERFACE: return DeviceType::UNDEFINED_CLASS;
        case LIBUSB_CLASS_AUDIO: return DeviceType::USB_CLASS_AUDIO;
        case LIBUSB_CLASS_COMM: return DeviceType::USB_CLASS_COMM;
        case LIBUSB_CLASS_HID: return DeviceType::USB_CLASS_HID;
        case LIBUSB_CLASS_PHYSICAL: return DeviceType::USB_CLASS_PHYSICAL;
        case LIBUSB_CLASS_PRINTER: return DeviceType::USB_CLASS_PRINTER;
        case LIBUSB_CLASS_MASS_STORAGE: return DeviceType::USB_CLASS_MASS_STORAGE;
        case LIBUSB_CLASS_HUB: return DeviceType::USB_CLASS_HUB;
        case LIBUSB_CLASS_DATA: return DeviceType::USB_CLASS_DATA;
        case LIBUSB_CLASS_SMART_CARD: return DeviceType::USB_CLASS_SMART_CARD;
        case LIBUSB_CLASS_CONTENT_SECURITY: return DeviceType::USB_CLASS_CONTENT_SECURITY;
        case LIBUSB_CLASS_VIDEO: return DeviceType::USB_CLASS_VIDEO;
        case LIBUSB_CLASS_PERSONAL_HEALTHCARE: return DeviceType::USB_CLASS_PERSONAL_HEALTHCARE;
        case LIBUSB_CLASS_DIAGNOSTIC_DEVICE: return DeviceType::USB_CLASS_DIAGNOSTIC_DEVICE;
        case LIBUSB_CLASS_WIRELESS: return DeviceType::USB_CLASS_WIRELESS;
        case LIBUSB_CLASS_MISCELLANEOUS: return DeviceType::USB_CLASS_MISCELLANEOUS;
        case LIBUSB_CLASS_APPLICATION: return DeviceType::USB_CLASS_APPLICATION;
        case LIBUSB_CLASS_VENDOR_SPEC: return DeviceType::USB_CLASS_VENDOR_SPEC;
        default: return DeviceType::UNDEFINED_CLASS;
    }
}


QT_USB_NAMESPACE_END
