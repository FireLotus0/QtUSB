#pragma once

#include "usbmonitor/impl/monitorbase.h"
#include "QtUsb/libusb.h"
#include <qmap.h>

QT_USB_NAMESPACE_BEGIN
class MonitorHotplug : public MonitorBase {
public:
    explicit MonitorHotplug(UsbMonitor* usbMonitor, QObject *parent = nullptr);

    ~MonitorHotplug() final;

    void addMonitorId(UsbId id) override;

    void removeMonitorId(UsbId id) override;

    void addMonitorClass(uint8_t devClass) override;

    void removeMonitorClass(uint8_t devClass) override;

private:
    QMap<UsbId, libusb_hotplug_callback_handle> idMonitorCbHandles;
    QMap<uint8_t, libusb_hotplug_callback_handle> classMonitorCbHandles;
};

int hotplugCallback(libusb_context *context, libusb_device *dev, libusb_hotplug_event event, void *user_data);

QT_USB_NAMESPACE_END
