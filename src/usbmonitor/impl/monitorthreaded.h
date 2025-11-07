#pragma once

#include "monitorbase.h"
#include "QtUsb/libusb.h"
#include "usbmonitor/usbmonitor.h"
#include <qobject.h>
#include <qthread.h>
#include <qmutex.h>
#include <qmap.h>

QT_USB_NAMESPACE_BEGIN
class MonitorWorker;

class MonitorThreaded : public MonitorBase {
public:
    explicit MonitorThreaded(UsbMonitor* usbMonitor, QObject *parent = nullptr);

    virtual ~MonitorThreaded();

    void addMonitorId(QT_USB::UsbId id) override;

    void removeMonitorId(QT_USB::UsbId id) override;

    void addMonitorClass(uint8_t devClass) override;

    void removeMonitorClass(uint8_t devClass) override;

    void startMonitor() override;

    void stopMonitor() override;

private:
    MonitorWorker* worker{nullptr};
    QThread *workThr;
    QAtomicInt workerEnable{0};
};

class MonitorWorker : public QObject {
    Q_OBJECT
public:
    explicit MonitorWorker(UsbMonitor* usbMonitor, QAtomicInt &workerEnable, QAtomicInt &monitorFlag, QObject *parent = nullptr);

signals:
    void startMonitor();
    void addMonitorId(QT_USB::UsbId id);
    void removeMonitorId(QT_USB::UsbId id);
    void addMonitorClass(uint8_t devClass);
    void removeMonitorClass(uint8_t devClass);

private slots:
    void onStartMonitor();
    void onAddMonitorId(QT_USB::UsbId id);
    void onRemoveMonitorId(QT_USB::UsbId id);
    void onAddMonitorClass(uint8_t devClass);
    void onRemoveMonitorClass(uint8_t devClass);

private:
    void updateMonitorIds();
    void updateMonitorClassed();

private:
    UsbMonitor* usbMonitor;
    UsbId usbIdTemp;
    libusb_device **devLists;
    QAtomicInt &workerEnable;
    QAtomicInt &monitorFlag;
    libusb_device_descriptor descriptor;
    QList<UsbId>  addIdCache, removeIdCache;
    QMap<UsbId, bool> monitorIds, searchCache;

    QList<uint8_t> addClassCache, removeClassCache;
    QSet<uint8_t> monitorClasses;
};

QT_USB_NAMESPACE_END
