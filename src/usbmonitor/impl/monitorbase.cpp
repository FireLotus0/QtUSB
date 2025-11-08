#include "monitorbase.h"
#include <qloggingcategory.h>

QT_USB_NAMESPACE_BEGIN
const QLoggingCategory &usbCategory();

MonitorBase::MonitorBase(UsbMonitor *usbMonitor, QObject *parent)
    : QObject(parent)
      , usbMonitor(usbMonitor) {
}

MonitorBase::~MonitorBase() {
}

void MonitorBase::startMonitor() {
    monitorFlag = 1;
    qCDebug(usbCategory) << "Monitor started";
}

void MonitorBase::stopMonitor() {
    qCDebug(usbCategory) << "Monitor stopped";
    monitorFlag = 0;
}

libusb_class_code MonitorBase::deviceType2Code(DeviceType deviceType) {
    switch (deviceType) {
        case DeviceType::UNDEFINED: return LIBUSB_CLASS_PER_INTERFACE;
        case DeviceType::USB_CLASS_AUDIO: return LIBUSB_CLASS_AUDIO;
        case DeviceType::USB_CLASS_COMM: return LIBUSB_CLASS_COMM;
        case DeviceType::USB_CLASS_HID: return LIBUSB_CLASS_HID;
        case DeviceType::USB_CLASS_PHYSICAL: return LIBUSB_CLASS_PHYSICAL;
        case DeviceType::USB_CLASS_IMAGE: return LIBUSB_CLASS_IMAGE;
        case DeviceType::USB_CLASS_PRINTER: return LIBUSB_CLASS_PRINTER;
        case DeviceType::USB_CLASS_MASS_STORAGE: return LIBUSB_CLASS_MASS_STORAGE;
        case DeviceType::USB_CLASS_HUB: return LIBUSB_CLASS_HUB;
        case DeviceType::USB_CLASS_DATA: return LIBUSB_CLASS_DATA;
        case DeviceType::USB_CLASS_SMART_CARD: return LIBUSB_CLASS_SMART_CARD;
        case DeviceType::USB_CLASS_CONTENT_SECURITY: return LIBUSB_CLASS_CONTENT_SECURITY;
        case DeviceType::USB_CLASS_VIDEO: return LIBUSB_CLASS_VIDEO;
        case DeviceType::USB_CLASS_PERSONAL_HEALTHCARE: return LIBUSB_CLASS_PERSONAL_HEALTHCARE;
        case DeviceType::USB_CLASS_DIAGNOSTIC_DEVICE: return LIBUSB_CLASS_DIAGNOSTIC_DEVICE;
        case DeviceType::USB_CLASS_WIRELESS: return LIBUSB_CLASS_WIRELESS;
        case DeviceType::USB_CLASS_MISCELLANEOUS: return LIBUSB_CLASS_MISCELLANEOUS;
        case DeviceType::USB_CLASS_APPLICATION: return LIBUSB_CLASS_APPLICATION;
        case DeviceType::USB_CLASS_VENDOR_SPEC: return LIBUSB_CLASS_VENDOR_SPEC;
        default: return LIBUSB_CLASS_PER_INTERFACE;
    }
}

QT_USB_NAMESPACE_END
