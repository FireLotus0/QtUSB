#include "monitorbase.h"
#include <qdebug.h>

QT_USB_NAMESPACE_BEGIN
MonitorBase::MonitorBase(UsbMonitor *usbMonitor, QObject *parent)
    : QObject(parent)
    , usbMonitor(usbMonitor) {
}

MonitorBase::~MonitorBase() {
}

void MonitorBase::startMonitor() {
    monitorFlag = 1;
    qDebug() << "Monitor started";
}

void MonitorBase::stopMonitor() {
    qDebug() << "Monitor stopped";
    monitorFlag = 0;
}

QT_USB_NAMESPACE_END
