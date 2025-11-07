#include "monitorhotplug.h"
#include "usbmonitor/usbmonitor.h"
#include <qloggingcategory.h>

QT_USB_NAMESPACE_BEGIN

const QLoggingCategory &usbCategory();

MonitorHotplug::MonitorHotplug(UsbMonitor* usbMonitor, QObject *parent) : MonitorBase(usbMonitor, parent) {
}

MonitorHotplug::~MonitorHotplug() {
    for (auto handle: idMonitorCbHandles.values()) {
        libusb_hotplug_deregister_callback(nullptr, handle);
    }
    for (auto handle: classMonitorCbHandles.values()) {
        libusb_hotplug_deregister_callback(nullptr, handle);
    }
}

void MonitorHotplug::addMonitorId(UsbId id) {
    if (idMonitorCbHandles.contains(id)) {
        return;
    }
    libusb_hotplug_callback_handle handle;
    auto rc = libusb_hotplug_register_callback(nullptr, LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED | LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT, 0, id.vid, id.pid,
                                               LIBUSB_HOTPLUG_MATCH_ANY, hotplugCallback, this, &handle);
    if (rc != LIBUSB_SUCCESS) {
        qCWarning(usbCategory) << "Register HotPlug failed: " << libusb_error_name(rc);
    } else {
        idMonitorCbHandles.insert(id, handle);
    }
}

void MonitorHotplug::removeMonitorId(UsbId id) {
    if (!idMonitorCbHandles.contains(id)) {
        return;
    }
    libusb_hotplug_deregister_callback(nullptr, idMonitorCbHandles.value(id));
    idMonitorCbHandles.remove(id);
}

void MonitorHotplug::addMonitorClass(uint8_t devClass) {
    if(classMonitorCbHandles.contains(devClass)) {
        return;
    }
    libusb_hotplug_callback_handle handle;
    auto rc = libusb_hotplug_register_callback(nullptr, LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED | LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT,
                                               0, LIBUSB_HOTPLUG_MATCH_ANY, LIBUSB_HOTPLUG_MATCH_ANY,
                                               devClass, hotplugCallback, this, &handle);
    if (rc != LIBUSB_SUCCESS) {
        qCWarning(usbCategory) << "addMonitorClass(): Register HotPlug failed: " << libusb_error_name(rc);
    } else {
        classMonitorCbHandles.insert(devClass, handle);
    }

}

void MonitorHotplug::removeMonitorClass(uint8_t devClass) {
    if(!classMonitorCbHandles.contains(devClass)) {
        return;
    }
    libusb_hotplug_deregister_callback(nullptr, classMonitorCbHandles.value(devClass));
    classMonitorCbHandles.remove(devClass);
}

int hotplugCallback(libusb_context *context, libusb_device *dev, libusb_hotplug_event event, void *user_data) {
    static libusb_device_handle *dev_handle = nullptr;
    libusb_device_descriptor desc;
    libusb_get_device_descriptor(dev, &desc);
    auto monitorHotPlug = (MonitorHotplug*)user_data;
    if (*((QAtomicInt*)user_data) == 1) {
        if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED) {
            libusb_ref_device(dev);
            monitorHotPlug->usbMonitor->deviceAttached({desc.idProduct, desc.idVendor}, {dev});
        } else if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT) {
            monitorHotPlug->usbMonitor->deviceDetached({desc.idProduct, desc.idVendor});
        } else {
            qCWarning(usbCategory) << "HotPlug callback returned an invalid event";
        }
    }
    return 0;
}

QT_USB_NAMESPACE_END
