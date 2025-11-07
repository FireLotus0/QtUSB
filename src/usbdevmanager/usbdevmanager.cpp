#include "QtUsb/usbdevmanager.h"
#include "usbmonitor/usbmonitor.h"
#include "QtUsb/usbdevice.h"
#include <qloggingcategory.h>

QT_USB_NAMESPACE_BEGIN

UsbDevManager::UsbDevManager(QObject *parent)
{
    libusb_context *ctx = nullptr;
    if (libusb_init(&ctx) < 0) {
        qCCritical(usbCategory) << "libusb_init failed";
    } else {
        qRegisterMetaType<UsbId>("UsbId");
        qRegisterMetaType<IoData>("RequestData");
        qRegisterMetaType<LibUsbDevWrap>("LibUsbDevWrap");
        qRegisterMetaType<IoData>("IoData");
        qRegisterMetaType<uint8_t>("uint8_t");

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
    devices[id] = QSharedPointer<UsbDevice>(new UsbDevice(id, dev.device, this));
    emit deviceAttached(id);
}

void UsbDevManager::onDeviceDetached(UsbId id) {
    Q_ASSERT(devices.contains(id));
    devices[id]->setValid(false);
    devices.remove(id);
    emit deviceDetached(id);
}

void UsbDevManager::setLogLevel(UsbLogLevel level) {
    QString rule = "usb.category.debug=true";
    switch((int)level) {
        case (int)UsbLogLevel::INFO: rule =  "usb.category.debug=false\n"
                                             "usb.category.info=true"; break;
        case (int)UsbLogLevel::WARNING: rule =  "usb.category.debug=false\n"
                                                "usb.category.info=false\n"
                                                "usb.category.warning=true";  break;
        case (int)UsbLogLevel::CRITICAL: rule =  "usb.category.debug=false\n"
                                                 "usb.category.info=false\n"
                                                 "usb.category.warning=false\n"
                                                 "usb.category.critical=true"; break;
    }
    QLoggingCategory::setFilterRules(rule);
}

void UsbDevManager::addMonitorClass(uint8_t devClass) {
    monitor->addMonitorClass(devClass);
}

void UsbDevManager::removeMonitorClass(uint8_t devClass) {
    monitor->removeMonitorClass(devClass);
}


QT_USB_NAMESPACE_END
