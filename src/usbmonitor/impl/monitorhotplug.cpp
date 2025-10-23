#include "monitorhotplug.h"
#include "src/usbmonitor/usbmonitor.h"
#include <qdebug.h>

QT_USB_NAMESPACE_BEGIN

QT_USB::MonitorHotplug::MonitorHotplug(QObject *parent)
{
}

MonitorHotplug::~MonitorHotplug() {
    for (auto handle: callbackHandles.values()) {
        libusb_hotplug_deregister_callback(nullptr, handle);
    }
}

void MonitorHotplug::addMonitorId(UsbId id) {
    if (callbackHandles.contains(id)) {
        return;
    }
    libusb_hotplug_callback_handle handle;
    auto rc = libusb_hotplug_register_callback(nullptr, LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED | LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT, 0, id.vid, id.pid,
                                               LIBUSB_HOTPLUG_MATCH_ANY, hotplugCallback, &monitorFlag, &handle);
    if (rc != LIBUSB_SUCCESS) {
        qWarning() << "Register HotPlug failed: " << libusb_error_name(rc);
    } else {
        callbackHandles.insert(id, handle);
    }
}

void MonitorHotplug::removeMonitorId(UsbId id) {
    if (!callbackHandles.contains(id)) {
        return;
    }
    libusb_hotplug_deregister_callback(nullptr, callbackHandles.value(id));
    callbackHandles.remove(id);
}

int hotplugCallback(libusb_context *context, libusb_device *dev, libusb_hotplug_event event, void *user_data) {
    static libusb_device_handle *dev_handle = nullptr;
    libusb_device_descriptor desc;
    libusb_get_device_descriptor(dev, &desc);
    if (*((QAtomicInt*)user_data) == 1) {
        if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED) {
            UsbMonitor::instance().deviceAttached({desc.idProduct, desc.idVendor}, {dev});
        } else if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT) {
            UsbMonitor::instance().deviceDetached({desc.idProduct, desc.idVendor});
        } else {
            qWarning() << "HotPlug callback returned an invalid event";
        }
    }
    return 0;
}

QT_USB_NAMESPACE_END
