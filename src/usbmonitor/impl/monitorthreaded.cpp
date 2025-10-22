#include "monitorthreaded.h"
#include "src/usbmonitor/usbmonitor.h"
#include <qdebug.h>
#include <QCoreApplication>

QT_USB_NAMESPACE_BEGIN

MonitorThreaded::MonitorThreaded(QObject *parent)
        : MonitorBase(parent) {
    workThr = new QThread(this);
    worker = new MonitorWorker(workerEnable, monitorFlag);
    worker->moveToThread(workThr);
    connect(workThr, &QThread::finished, worker, &QObject::deleteLater);
    workThr->start();
}

MonitorThreaded::~MonitorThreaded() {
    MonitorThreaded::stopMonitor();
    workThr->quit();
    workThr->wait();
    delete workThr;
}

void MonitorThreaded::addMonitorId(QT_USB::UsbId id) {
    worker->addMonitorId(id);
}

void MonitorThreaded::removeMonitorId(QT_USB::UsbId id) {
    worker->removeMonitorId(id);
}

void MonitorThreaded::startMonitor() {
    MonitorBase::startMonitor();
    workerEnable.storeRelaxed(1);
    worker->startMonitor();
}

void MonitorThreaded::stopMonitor() {
    MonitorBase::stopMonitor();
    workerEnable.storeRelaxed(0);
}

MonitorWorker::MonitorWorker(QAtomicInt &workerEnable, QAtomicInt &monitorFlag, QObject *parent)
        : QObject(parent), workerEnable(workerEnable), monitorFlag(monitorFlag) {
    connect(this, &MonitorWorker::startMonitor, this, &MonitorWorker::onStartMonitor);
    connect(this, &MonitorWorker::addMonitorId, this, &MonitorWorker::onAddMonitorId, Qt::QueuedConnection);
    connect(this, &MonitorWorker::removeMonitorId, this, &MonitorWorker::onRemoveMonitorId);
}

void MonitorWorker::onStartMonitor() {
    while (workerEnable.loadRelaxed() == 1 && monitorFlag.loadRelaxed() == 1) {
        int count = libusb_get_device_list(nullptr, &devLists);
        for(auto id : searchCache.keys()) {
            searchCache[id] = false;
        }
        for (int i = 0; i < count; i++) {
            auto ret = libusb_get_device_descriptor(devLists[i], &descriptor);
            if (ret >= 0) {
                usbIdTemp.pid = descriptor.idProduct;
                usbIdTemp.vid = descriptor.idVendor;
                if(monitorIds.contains(usbIdTemp)) {
                    searchCache[usbIdTemp] = true;
                    if(!monitorIds[usbIdTemp]) {
                        monitorIds[usbIdTemp] = true;
                        UsbMonitor::instance().deviceAttached(usbIdTemp);
                    }
                }
            } else {
                qDebug() << libusb_error_name(ret);
            }
        }
        for(auto id : searchCache.keys()) {
            if(!searchCache[id] && monitorIds[id]) {
                monitorIds[id] = false;
                UsbMonitor::instance().deviceDetached(id);
            }
        }
        libusb_free_device_list(devLists, 1);
        updateMonitorIds();
        QCoreApplication::processEvents(QEventLoop::AllEvents, 10);
        QThread::msleep(500);
    }
}

void MonitorWorker::onAddMonitorId(QT_USB::UsbId id) {
    addCache.append(id);
}

void MonitorWorker::onRemoveMonitorId(QT_USB::UsbId id) {
    removeCache.append(id);
}

void MonitorWorker::updateMonitorIds() {
    if (!addCache.isEmpty()) {
        for (const auto &addId: addCache) {
            if (!monitorIds.contains(addId)) {
                monitorIds[addId] = false;
            }
        }
        addCache.clear();
    }
    if (!removeCache.isEmpty()) {
        for (const auto &removeId: removeCache) {
            if (monitorIds.contains(removeId)) {
                monitorIds.remove(removeId);
            }
        }
        removeCache.clear();
    }
}

QT_USB_NAMESPACE_END
