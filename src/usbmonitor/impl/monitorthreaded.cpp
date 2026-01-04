#include "monitorthreaded.h"
#include "usbmonitor/usbmonitor.h"
#include <QCoreApplication>
#include <qloggingcategory.h>

#include "descriptor/usbdescriptor.h"

QT_USB_NAMESPACE_BEGIN
MonitorThreaded::MonitorThreaded(UsbMonitor *usbMonitor, QObject *parent)
    : MonitorBase(usbMonitor, parent) {
    workThr = new QThread(this);
    worker = new MonitorWorker(usbMonitor, workerEnable, monitorFlag);
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

void MonitorThreaded::addMonitorClass(DeviceType deviceType) {
    worker->addMonitorClass(deviceType);
}

void MonitorThreaded::removeMonitorClass(DeviceType deviceType) {
    worker->removeMonitorClass(deviceType);
}

MonitorWorker::MonitorWorker(UsbMonitor* usbMonitor, QAtomicInt &workerEnable, QAtomicInt &monitorFlag, QObject *parent)
    : QObject(parent), workerEnable(workerEnable), monitorFlag(monitorFlag), usbMonitor(usbMonitor) {
    connect(this, &MonitorWorker::startMonitor, this, &MonitorWorker::onStartMonitor);
    connect(this, &MonitorWorker::addMonitorId, this, &MonitorWorker::onAddMonitorId);
    connect(this, &MonitorWorker::removeMonitorId, this, &MonitorWorker::onRemoveMonitorId);

    connect(this, &MonitorWorker::addMonitorClass, this, &MonitorWorker::onAddMonitorClass);
    connect(this, &MonitorWorker::removeMonitorClass, this, &MonitorWorker::onRemoveMonitorClass);
}

void MonitorWorker::onStartMonitor() {
    while (workerEnable.loadRelaxed() == 1 && monitorFlag.loadRelaxed() == 1) {
        int count = libusb_get_device_list(nullptr, &devLists);
        for (auto id: searchCache.keys()) {
            searchCache[id] = false;
        }
        for (int i = 0; i < count; i++) {
            auto ret = libusb_get_device_descriptor(devLists[i], &descriptor);
            if (ret >= 0) {
                usbIdTemp.pid = descriptor.idProduct;
                usbIdTemp.vid = descriptor.idVendor;
                if (!UsbDescriptor::descriptors.contains(usbIdTemp)) {
                    UsbDescriptor::descriptors.insert(usbIdTemp, UsbDescriptor(devLists[i]));
                }
                for (const auto& type : monitorClasses) {
                    if (!monitorIds.contains(usbIdTemp) && UsbDescriptor::descriptors[usbIdTemp].getDeviceTypes().testFlag(type)) {
                            monitorIds[usbIdTemp] = false;
                    }
                }
                if (monitorIds.contains(usbIdTemp)) {
                    searchCache[usbIdTemp] = true;
                    if (!monitorIds[usbIdTemp]) {
                        monitorIds[usbIdTemp] = true;
                        usbMonitor->deviceAttached(usbIdTemp);
                    }
                }
            } else {
                qCWarning(usbCategory) << libusb_error_name(ret);
            }
        }
        for (auto id: searchCache.keys()) {
            if (!searchCache[id] && monitorIds[id]) {
                monitorIds[id] = false;
                usbMonitor->deviceDetached(id);
            }
        }
//         libusb_free_device_list(devLists, 1);
        updateMonitorIds();
        updateMonitorClassed();
        QCoreApplication::processEvents(QEventLoop::AllEvents, 10);
        QThread::msleep(500);
    }
}

void MonitorWorker::onAddMonitorId(QT_USB::UsbId id) {
    addIdCache.append(id);
}

void MonitorWorker::onRemoveMonitorId(QT_USB::UsbId id) {
    removeIdCache.append(id);
}

void MonitorWorker::updateMonitorIds() {
    if (!addIdCache.isEmpty()) {
        for (const auto &addId: addIdCache) {
            if (!monitorIds.contains(addId)) {
                monitorIds[addId] = false;
            }
        }
        addIdCache.clear();
    }
    if (!removeIdCache.isEmpty()) {
        for (const auto &removeId: removeIdCache) {
            if (monitorIds.contains(removeId)) {
                monitorIds.remove(removeId);
            }
        }
        removeIdCache.clear();
    }
}

void MonitorWorker::updateMonitorClassed() {
    for(auto devClass : addClassCache) {
        monitorClasses.insert(devClass);
    }
    for(auto devClass : removeClassCache) {
        monitorClasses.remove(devClass);
    }
}

void MonitorWorker::onAddMonitorClass(DeviceType deviceType) {
    addClassCache.append(deviceType);
}

void MonitorWorker::onRemoveMonitorClass(DeviceType deviceType) {
    removeClassCache.removeAll(deviceType);
}


QT_USB_NAMESPACE_END
