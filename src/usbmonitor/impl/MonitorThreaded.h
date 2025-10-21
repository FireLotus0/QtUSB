#pragma once

#include "monitorbase.h"
#include <qobject.h>
#include <qthread.h>
#include <qmutex.h>

#include "usbmanager/libusb.h"

QT_USB_NAMESPACE_BEGIN
class MonitorWorker;

class MonitorThreaded : public MonitorBase {
public:
    explicit MonitorThreaded(QObject *parent = nullptr);

    virtual ~MonitorThreaded();

    void addMonitorId(QT_USB::UsbId id) override;

    void removeMonitorId(QT_USB::UsbId id) override;

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
    explicit MonitorWorker(QAtomicInt &workerEnable, QAtomicInt &monitorFlag, QObject *parent = nullptr);

signals:
    void startMonitor();
    void addMonitorId(QT_USB::UsbId id);
    void removeMonitorId(QT_USB::UsbId id);

private slots:
    void onStartMonitor();
    void onAddMonitorId(QT_USB::UsbId id);
    void onRemoveMonitorId(QT_USB::UsbId id);

private:
    void updateMonitorIds();

private:
    libusb_device **devLists;
    QAtomicInt &workerEnable;
    QAtomicInt &monitorFlag;
    QList<UsbId> monitorIds, addCache, removeCache, searchCache;
};

QT_USB_NAMESPACE_END
