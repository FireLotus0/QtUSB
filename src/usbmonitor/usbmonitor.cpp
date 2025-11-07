#include "usbmonitor.h"
#include "QtUsb/libusb.h"
#include "usbmonitor/impl/monitorthreaded.h"
#include "usbmonitor/impl/monitorhotplug.h"

QT_USB_NAMESPACE_BEGIN

UsbMonitor::UsbMonitor(QObject *parent)
        : QObject(parent)
{
    initMonitor();
}

UsbMonitor::~UsbMonitor() {
    stop();
    delete monitor;
}

void UsbMonitor::start() const {
    monitor->startMonitor();
}

void UsbMonitor::stop() const {
    monitor->stopMonitor();
}

void UsbMonitor::initMonitor() {
    if(libusb_has_capability(LIBUSB_CAP_HAS_HOTPLUG)) {
        monitor = new MonitorHotplug(this);
    } else {
        monitor = new MonitorThreaded(this);
    }
}

void UsbMonitor::addMonitorId(UsbId id) {
    monitor->addMonitorId(id);
}

void UsbMonitor::removeMonitorId(UsbId id) {
    monitor->removeMonitorId(id);
}

void UsbMonitor::addMonitorClass(uint8_t devClass) {
    monitor->addMonitorClass(devClass);
}

void UsbMonitor::removeMonitorClass(uint8_t devClass) {
    monitor->removeMonitorClass(devClass);
}

QT_USB_NAMESPACE_END