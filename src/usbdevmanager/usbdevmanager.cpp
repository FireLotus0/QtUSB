#include "../../include/QtUsb/usbdevmanager.h"
#include "src/usbmonitor/usbmonitor.h"
#include "../../include/QtUsb/usbdevice.h"
#include <qdebug.h>

QT_USB_NAMESPACE_BEGIN

UsbDevManager::UsbDevManager(QObject *parent)
{
    libusb_context *ctx = nullptr;
    if (libusb_init(&ctx) < 0) {
        qWarning() << "libusb_init failed";
    } else {
        qRegisterMetaType<UsbId>("UsbId");
        qRegisterMetaType<IoData>("RequestData");
        qRegisterMetaType<LibUsbDevWrap>("LibUsbDevWrap");
        qRegisterMetaType<IoData>("IoData");

        monitor = new UsbMonitor(parent);
        connect(monitor, &UsbMonitor::deviceAttached, this, &UsbDevManager::onDeviceAttached);
        connect(monitor, &UsbMonitor::deviceDetached, this, &UsbDevManager::onDeviceDetached);
        monitor->start();
    }
}

UsbDevManager::~UsbDevManager() {
    for (auto device : devices) {
        device->setValid(false);
    }
    devices.clear();
    delete monitor;
    libusb_exit(nullptr);
}

UsbDevManager & UsbDevManager::instance() {
    static UsbDevManager instance;
    return instance;
}

void UsbDevManager::addMonitorId(UsbId id) {
    monitor->addMonitorId(id);
}

void UsbDevManager::removeMonitorId(UsbId id) {
    monitor->removeMonitorId(id);
}

QSharedPointer<UsbDevice> UsbDevManager::getDevice(UsbId id) const {
    if (devices.contains(id)) {
        return devices.value(id);
    }
    return nullptr;
}

void UsbDevManager::onDeviceAttached(UsbId id, LibUsbDevWrap dev) {
    Q_ASSERT(!devices.contains(id));
    devices[id] = QSharedPointer<UsbDevice>(new UsbDevice(this));
    devices[id]->openDevice(id, dev.device);
    emit deviceAttached(id);
}

void UsbDevManager::onDeviceDetached(UsbId id) {
    Q_ASSERT(devices.contains(id));
    devices[id]->setValid(false);
    devices.remove(id);
    emit deviceDetached(id);
}


QT_USB_NAMESPACE_END
