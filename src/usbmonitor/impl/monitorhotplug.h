#pragma once

#include "src/usbmonitor/impl/monitorbase.h"
#include "src/libusb.h"
#include <qmap.h>

QT_USB_NAMESPACE_BEGIN
class MonitorHotplug : public MonitorBase {
public:
    explicit MonitorHotplug(QObject *parent = nullptr);

    ~MonitorHotplug() final;

    void addMonitorId(UsbId id) override;

    void removeMonitorId(UsbId id) override;

private:
    QMap<UsbId, libusb_hotplug_callback_handle> callbackHandles;
};

int hotplugCallback(libusb_context *context, libusb_device *dev, libusb_hotplug_event event, void *user_data);

QT_USB_NAMESPACE_END
