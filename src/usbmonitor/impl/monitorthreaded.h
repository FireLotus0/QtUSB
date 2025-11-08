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

    void addMonitorClass(DeviceType deviceType) override;

    void removeMonitorClass(DeviceType deviceType) override;

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
    void addMonitorClass(DeviceType deviceType);
    void removeMonitorClass(DeviceType deviceType);

private slots:
    void onStartMonitor();
    void onAddMonitorId(QT_USB::UsbId id);
    void onRemoveMonitorId(QT_USB::UsbId id);
    void onAddMonitorClass(DeviceType deviceType);
    void onRemoveMonitorClass(DeviceType deviceType);

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

    QList<DeviceType> addClassCache, removeClassCache;
    QSet<DeviceType> monitorClasses;
};

inline uint qHash(DeviceType key, uint seed = 0) noexcept
{
    return ::qHash(static_cast<quint64>(key), seed);
}

QT_USB_NAMESPACE_END
