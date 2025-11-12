#pragma once

#include "usbmonitor/impl/monitorbase.h"
#include "QtUsb/libusb.h"
#include <qmap.h>
#include <qtimer.h>

QT_USB_NAMESPACE_BEGIN
class MonitorHotplug : public MonitorBase {
public:
    explicit MonitorHotplug(UsbMonitor* usbMonitor, QObject *parent = nullptr);

    ~MonitorHotplug() final;

    void addMonitorId(UsbId id) override;

    void removeMonitorId(UsbId id) override;

    void addMonitorClass(DeviceType deviceType) override;

    void removeMonitorClass(DeviceType deviceType) override;

private:
    QMap<UsbId, libusb_hotplug_callback_handle> idMonitorCbHandles;
    QMap<DeviceType, libusb_hotplug_callback_handle> classMonitorCbHandles;
    struct timeval timeout;
    QTimer eventHandleTimer;
};

int hotplugCallback(libusb_context *context, libusb_device *dev, libusb_hotplug_event event, void *user_data);

QT_USB_NAMESPACE_END
