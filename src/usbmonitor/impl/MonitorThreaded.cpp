#include "MonitorThreaded.h"

QT_USB_NAMESPACE_BEGIN

MonitorThreaded::MonitorThreaded(QObject *parent)
        : MonitorBase(parent)
{
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
    : QObject(parent), workerEnable(workerEnable), monitorFlag(monitorFlag)
{
    connect(this, &MonitorWorker::startMonitor, this, &MonitorWorker::onStartMonitor);
    connect(this, &MonitorWorker::addMonitorId, this, &MonitorWorker::onAddMonitorId);
    connect(this, &MonitorWorker::removeMonitorId, this, &MonitorWorker::onRemoveMonitorId);
}

void MonitorWorker::onStartMonitor() {
    while (workerEnable.loadRelaxed() == 1 && monitorFlag.loadRelaxed() == 1) {
        searchCache.clear();
        devCount = libusb_get_device_list(nullptr, &devLists);
        tmpDevices.data.clear();
        if (devCount <= 0) {
            emit deviceChanged(tmpDevices);
        } else if(devCount != devices.data.size()) {
            isInsert = devCount > devices.data.size();
            // if(isInsert) {
            for (int i = 0; i < devCount; i++) {
                // 获取PID，VID
                auto ret = libusb_get_device_descriptor(devLists[i], &deviceDescriptor);
                if(ret >= 0) {
                    auto pairId = qMakePair(deviceDescriptor.idProduct, deviceDescriptor.idVendor);
                    if(!devices.data.contains(pairId)) {
                        tmpDevices.data[pairId] = devLists[i];
                        devices.data[pairId] = devLists[i];
                    }
                }
            }
            if(!tmpDevices.data.isEmpty()) {
                emit deviceChanged(tmpDevices);
            }
        } else {
            tmpDevices.data = devices.data;
            for (int i = 0; i < devCount; i++) {
                auto ret = libusb_get_device_descriptor(devLists[i], &deviceDescriptor);
                if(ret >= 0) {
                    tmpDevices.data.remove(qMakePair(deviceDescriptor.idProduct, deviceDescriptor.idVendor));
                }
            }
            for(const auto& key : tmpDevices.data.keys()) {
                devices.data.remove(key);
            }
            emit deviceChanged(tmpDevices);
        }
    }
    libusb_free_device_list(devLists, 1);

        updateMonitorIds();
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
        for (const auto &addId : addCache) {
            if (!monitorIds.contains(addId)) {
                monitorIds.append(addId);
            }
        }
        addCache.clear();
    }
   if (!removeCache.isEmpty()) {
       for (const auto &removeId : removeCache) {
           if (monitorIds.contains(removeId)) {
               monitorIds.removeAll(removeId);
           }
       }
       removeCache.clear();
   }
}

QT_USB_NAMESPACE_END
