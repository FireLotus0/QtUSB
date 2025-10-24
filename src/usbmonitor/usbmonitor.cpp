#include "usbmonitor.h"
#include "src/libusb.h"
#include "src/usbmonitor/impl/monitorthreaded.h"
#include "src/usbmonitor/impl/monitorhotplug.h"

QT_USB_NAMESPACE_BEGIN

UsbMonitor::UsbMonitor(QObject *parent)
        : QObject(parent)
{
    initMonitor();
}

UsbMonitor &UsbMonitor::instance() {
    static UsbMonitor monitor;
    return monitor;
}

void UsbMonitor::start() {
    monitor->startMonitor();
}

void UsbMonitor::stop() {
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

QT_USB_NAMESPACE_END