#include "periodicreader.h"

QT_USB_NAMESPACE_BEGIN

PeriodicReadWorker::PeriodicReadWorker(IoCommand* ioCmd, QObject *parent)
    : QObject(parent)
    , ioCmd(ioCmd)
{
    connect(this, &PeriodicReadWorker::startTimer, this, &PeriodicReadWorker::onStartTimer);
}

PeriodicReadWorker::~PeriodicReadWorker() {
    if (timer) {
        timer->stop();
    }
}

void PeriodicReadWorker::onStartTimer(bool start, int interval) {
    if (!start) {
        if (timer) {
            timer->stop();
        }
        qCInfo(usbCategory) << "Stop periodic read!";
    } else {
        if (!timer) {
            timer = new QTimer(this);
            timer->callOnTimeout([this] {
               ioCmd->periodicRead();
            });
        }
        if (interval > 0) {
            timer->setInterval(interval);
            timer->start();
            qCInfo(usbCategory) << "Start periodic read: interval=" << interval << "ms";
        }
    }
}

PeriodicReader::PeriodicReader(IoCommand* ioCmd, QObject* parent)
    : QObject(parent)
{
    worker = new PeriodicReadWorker(ioCmd);
    thr = new QThread;
    worker->moveToThread(thr);
    connect(thr, &QThread::finished, worker, &QObject::deleteLater);
    thr->start();
}

PeriodicReader::~PeriodicReader() {
    worker->startTimer(false);
    thr->quit();
    thr->wait();
    delete thr;
}

void PeriodicReader::startPeriodicRead(bool start, int interval) {
    worker->startTimer(start, interval);
}


QT_USB_NAMESPACE_END
