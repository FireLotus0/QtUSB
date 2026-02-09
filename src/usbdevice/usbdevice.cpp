#include "QtUsb/usbdevice.h"
#include "iocommand/iocommand.h"
#include "descriptor/usbdescriptor.h"
#include <qloggingcategory.h>

QT_USB_NAMESPACE_BEGIN

UsbDevice::UsbDevice(UsbId usbId, QObject *parent)
    : QObject(parent)
    , id(usbId)
{
}

UsbDevice::~UsbDevice() {
    if (ioCommand) {
        delete ioCommand;
    }
    if (usbCfg.interface != 0xFF) {
        libusb_release_interface(handle, usbCfg.interface);
    }
    if (handle) {
        libusb_close(handle);
    }
    qCDebug(usbCategory) << "UsbDevice::~UsbDevice: " << id;
}

void UsbDevice::setValid(bool valid) {
    bool value = validFlag.load(std::memory_order_relaxed);
    while (!validFlag.compare_exchange_weak(value, valid, std::memory_order_release, std::memory_order_relaxed)) {
    };
}

void UsbDevice::setConfiguration(ActiveUSBConfig newCfg) {
    openDevice();

    if (!validFlag.load(std::memory_order_relaxed)) {
        qCWarning(usbCategory) << "UsbDevice::setConfiguration: device is invalid!";
        return;
    }
    if (usbCfg.interface != 0xFF) {
        libusb_release_interface(handle, usbCfg.interface);
    }
    libusb_set_configuration(handle, usbCfg.configuration);
    auto  rt = libusb_claim_interface(handle, newCfg.interface);
    if(rt != 0) {
        qCWarning(usbCategory) << "UsbDevice::setConfiguration: claim interface failed, rt=" << rt << " " << libusb_error_name(rt);
    } else {
        usbCfg = newCfg;
        ioCommand->setConfiguration(newCfg);
    }
}

void UsbDevice::read() const {
    if(checkIOEnabled(true)) {
        ioCommand->read();
    }
}

void UsbDevice::write(QByteArray &&data) const {
    if(checkIOEnabled(false)) {
        ioCommand->write(std::move(data));
    }
}

void UsbDevice::openDevice() {
    if(validFlag.load(std::memory_order_relaxed)) {
        return;
    }
    if (handle) {
        qCWarning(usbCategory) << "UsbDevice::openDevice: libusb device already open";
        return;
    }
    handle = libusb_open_device_with_vid_pid(nullptr, id.vid, id.pid);
    if (!handle) {
        qCWarning(usbCategory) << "Open device failed: " << id;
        return;
    }
    libusb_set_auto_detach_kernel_driver(handle, 1);
    const auto& descriptorData = UsbDescriptor::descriptors[id];
    ioCommand = new IoCommand(descriptorData.getDescriptorData(), handle, this);
    setValid(true);
}

void UsbDevice::setSpeedPrintEnable(bool readSpeed, bool writeSpeed) {
    if(ioCommand) {
        ioCommand->setSpeedPrintEnable(readSpeed, writeSpeed);
    }
}

void UsbDevice::printInfo() const {
    if(UsbDescriptor::descriptors.contains(id)) {
        UsbDescriptor::descriptors[id].printInfo();
    } else {
        qCWarning(usbCategory) << "UsbDescriptor::descriptors do not contains id: id=" << id;
    }
}

bool UsbDevice::checkIOEnabled(bool isRead) const {
    if (!validFlag.load(std::memory_order_relaxed)) {
        qCWarning(usbCategory) << "UsbDevice::setConfiguration: device is invalid!";
        return false;
    }
    if (usbCfg.interface == 0xFF) {
        qCWarning(usbCategory) << QString("UsbDevice::%1: usb configuration is not set!").arg(isRead ? "read" : "write");
        return false;
    }
    return true;
}

void UsbDevice::write(const QByteArray &data) const {
    if(checkIOEnabled(false)) {
        ioCommand->write(data);
    }
}

bool UsbDevice::isDevValid() const {
    return validFlag.load(std::memory_order_relaxed);
}

UsbId UsbDevice::getUsbId() const {
    return id;
}

ActiveUSBConfig UsbDevice::getCurCfg() const {
    return usbCfg;
}

QT_USB_NAMESPACE_END
