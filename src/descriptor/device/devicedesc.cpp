#include "devicedesc.h"
#include "descriptor/configuration/configdesc.h"
#include <qloggingcategory.h>

QT_USB_NAMESPACE_BEGIN

const QLoggingCategory &usbCategory();

DeviceDesc::DeviceDesc(libusb_device *device, DescriptorData* descriptorData)
    : DescriptorBase(device)
    , descriptorData(descriptorData)
{
    descriptorType = DescriptorType::DEVICE_DESCRIPTOR;
    printPrefix = QString(static_cast<int>(descriptorType), ' ');
}

void DeviceDesc::resolveInfo() {
    if(!children.isEmpty()) {
        return;
    }
    auto rc = libusb_get_device_descriptor(device, &desc);
    if (rc != LIBUSB_SUCCESS) {
        qCWarning(usbCategory) << "Resolve device descriptor failed: " << libusb_error_name(rc);
    } else {
        auto speed = libusb_get_device_speed(device);
        content += "USB<PID:" + QString::number(desc.idProduct) + " VID:" + QString::number(desc.idVendor) + ">" + "\n";
        content += genContentLine("Device Class: ", (devClassInfo.contains(desc.bDeviceClass) ? devClassInfo[desc.bDeviceClass] : "Unknown Device"));
        content += genContentLine("USB Version: ", QString::number(desc.bcdUSB >> 8), ".", QString::number(desc.bcdUSB & 0xFF));
        content += genContentLine("Device Speed: ", (devSpeedInfo.contains(speed) ? devSpeedInfo[speed] : "Unknown Speed"));
        content += genContentLine("Possible Configurations: ", QString::number(desc.bNumConfigurations));
        if((desc.bcdUSB >> 8) >= 3) {
            descriptorData->fullDuplexSupported = true;
        }
        descriptorData->deviceClass = desc.bDeviceClass;
        descriptorData->deviceSubClass = desc.bDeviceSubClass;
        descriptorData->deviceProtocol = desc.bDeviceProtocol;
        for(int i= 0; i < desc.bNumConfigurations; i++) {
            auto child = new ConfigDesc(device, i, descriptorData);
            child->resolveInfo();
            children.append(child);
        }
    }
}

QMap<uint8_t, QString> DeviceDesc::devClassInfo = {
        DESCRIPTOR_DESCRIPTION(0x00, "Use class code info from Interface Descriptors"),
        DESCRIPTOR_DESCRIPTION(0x01, "Audio device"),
        DESCRIPTOR_DESCRIPTION(0x02, "Communications and CDC Control"),
        DESCRIPTOR_DESCRIPTION(0x03, "Human Interface Device"),
        DESCRIPTOR_DESCRIPTION(0x05, "Physical device class"),
        DESCRIPTOR_DESCRIPTION(0x06, "Still Imaging device"),
        DESCRIPTOR_DESCRIPTION(0x07, "Printer device"),
        DESCRIPTOR_DESCRIPTION(0x08, "Mass Storage device"),
        DESCRIPTOR_DESCRIPTION(0x09, "Hub"),
        DESCRIPTOR_DESCRIPTION(0x0A, "CDC data device"),
        DESCRIPTOR_DESCRIPTION(0x0B, "Smart Card device"),
        DESCRIPTOR_DESCRIPTION(0x0D, "Content Security device"),
        DESCRIPTOR_DESCRIPTION(0x0E, "Video device"),
        DESCRIPTOR_DESCRIPTION(0x0F, "Personal Healthcare device"),
        DESCRIPTOR_DESCRIPTION(0x10, "Audio/Video Devices"),
        DESCRIPTOR_DESCRIPTION(0x11, "Billboard Device"),
        DESCRIPTOR_DESCRIPTION(0x12, "USB Type-C Bridge Device"),
        DESCRIPTOR_DESCRIPTION(0x13, "USB Bulk Display Protocol Device Class"),
        DESCRIPTOR_DESCRIPTION(0x14, "MCTP over USB Protocol Endpoint Device Class"),
        DESCRIPTOR_DESCRIPTION(0x3C, "I3C Device Class"),
        DESCRIPTOR_DESCRIPTION(0xDC, "Diagnostic Device"),
        DESCRIPTOR_DESCRIPTION(0xE0, "Wireless Controller"),
        DESCRIPTOR_DESCRIPTION(0xEF, "Miscellaneous"),
        DESCRIPTOR_DESCRIPTION(0xFE, "Application Specific"),
        DESCRIPTOR_DESCRIPTION(0xFF, "Vendor Specific"),
};

QMap<uint8_t, QString> DeviceDesc::devSpeedInfo = {
        DESCRIPTOR_DESCRIPTION(libusb_speed::LIBUSB_SPEED_UNKNOWN, "The OS doesn't report or know the device speed."),
        DESCRIPTOR_DESCRIPTION(libusb_speed::LIBUSB_SPEED_LOW, "The device is operating at low speed (1.5MBit/s)."),
        DESCRIPTOR_DESCRIPTION(libusb_speed::LIBUSB_SPEED_FULL, "The device is operating at full speed (12MBit/s)."),
        DESCRIPTOR_DESCRIPTION(libusb_speed::LIBUSB_SPEED_HIGH, "The device is operating at high speed (480MBit/s)."),
        DESCRIPTOR_DESCRIPTION(libusb_speed::LIBUSB_SPEED_SUPER, "The device is operating at super speed (5000MBit/s)."),
        DESCRIPTOR_DESCRIPTION(libusb_speed::LIBUSB_SPEED_SUPER_PLUS, "The device is operating at super speed plus (10000MBit/s)."),
};


QT_USB_NAMESPACE_END

