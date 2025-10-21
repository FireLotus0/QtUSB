#pragma once

#include "monitorbase.h"
#include <qobject.h>
#include <qthread.h>

QT_USB_NAMESPACE_BEGIN

class MonitorWin : public MonitorBase {
public:
    explicit MonitorWin(QObject *parent = nullptr);

    void addMonitor(QT_USB::UsbId id) override;

    void removeMonitor(QT_USB::UsbId id) override;
};

QT_USB_NAMESPACE_END