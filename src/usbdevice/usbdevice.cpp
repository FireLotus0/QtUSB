#include "../../include/QtUsb/usbdevice.h"
#include "src/iocommand/iocommand.h"
#include "src/descriptor/usbdescriptor.h"
#include <qdebug.h>

QT_USB_NAMESPACE_BEGIN
UsbDevice::UsbDevice(QObject *parent)
    : QObject(parent) {
}


UsbDevice::~UsbDevice() {
    if (ioCommand) {
        delete ioCommand;
    }
    if (descriptor) {
        delete descriptor;
    }
    if (usbCfg.interface != 0xFF) {
        libusb_release_interface(handle, usbCfg.interface);
    }
    if (handle) {
        libusb_close(handle);
    }
    qDebug() << "UsbDevice::~UsbDevice: " << id;
}

void UsbDevice::setValid(bool valid) {
    bool value = validFlag.load(std::memory_order_relaxed);
    while (!validFlag.compare_exchange_weak(value, valid, std::memory_order_release, std::memory_order_relaxed)) {
    };
}

void UsbDevice::setConfiguration(ActiveUSBConfig newCfg) {
    if (!validFlag.load(std::memory_order_relaxed)) {
        qWarning() << "UsbDevice::setConfiguration: device is invalid!";
        return;
    }
    if (usbCfg.interface != 0xFF) {
        libusb_release_interface(handle, usbCfg.interface);
    }
    libusb_set_configuration(handle, usbCfg.interface);
    /// TODO: Claim interface may failed on unix!
    libusb_claim_interface(handle, newCfg.interface);
    usbCfg = newCfg;
    ioCommand->setConfiguration(newCfg);
}

void UsbDevice::read() const {
    if (!validFlag.load(std::memory_order_relaxed)) {
        qWarning() << "UsbDevice::setConfiguration: device is invalid!";
        return;
    }
    if (usbCfg.interface == 0xFF) {
        qWarning() << "UsbDevice::read: usb configuration is not set!";
        return;
    }
    ioCommand->read();
}

void UsbDevice::write(QByteArray &&data) const {
    if (!validFlag.load(std::memory_order_relaxed)) {
        qWarning() << "UsbDevice::setConfiguration: device is invalid!";
        return;
    }
    if (usbCfg.interface == 0xFF) {
        qWarning() << "UsbDevice::write: usb configuration is not set!";
        return;
    }
    ioCommand->write(std::move(data));
}

void UsbDevice::openDevice(UsbId usbId, libusb_device *device) {
    if (handle) {
        qWarning() << "UsbDevice::openDevice: libusb device already open";
        return;
    }
    id = usbId;
    handle = libusb_open_device_with_vid_pid(nullptr, id.vid, id.pid);
    if (!handle) {
        qInfo() << "Open device failed: " << id;
        return;
    }
    libusb_set_auto_detach_kernel_driver(handle, 1);
    descriptor = new UsbDescriptor(device);
    ioCommand = new IoCommand(descriptor->getDescriptorData(), handle);
    descriptor->printInfo();
    connect(ioCommand, &IoCommand::readFinished, this, &UsbDevice::readFinished);
    connect(ioCommand, &IoCommand::writeFinished, this, &UsbDevice::writeFinished);
    connect(ioCommand, &IoCommand::errorOccurred, this, &UsbDevice::errorOccurred);
    setValid(true);
}

QT_USB_NAMESPACE_END
