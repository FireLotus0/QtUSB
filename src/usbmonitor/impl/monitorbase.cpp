#include "monitorbase.h"
#include <qloggingcategory.h>

QT_USB_NAMESPACE_BEGIN

const QLoggingCategory &usbCategory();

MonitorBase::MonitorBase(UsbMonitor *usbMonitor, QObject *parent)
    : QObject(parent)
    , usbMonitor(usbMonitor) {
}

MonitorBase::~MonitorBase() {
}

void MonitorBase::startMonitor() {
    monitorFlag = 1;
    qCDebug(usbCategory) << "Monitor started";
}

void MonitorBase::stopMonitor() {
    qCDebug(usbCategory) << "Monitor stopped";
    monitorFlag = 0;
}

QT_USB_NAMESPACE_END
