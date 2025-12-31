#include "transfercontext.h"
#include "transfer/impl/sync/bulk/syncbulktransfer.h"
#include "transfer/impl/sync/control/synccontroltransfer.h"
#include "transfer/impl/sync/interrupt/syncintertransfer.h"
#include <qthread.h>
#include <qloggingcategory.h>

QT_USB_NAMESPACE_BEGIN

const QLoggingCategory &usbCategory();

TransferContext::TransferContext(uint8_t discardBytes, QObject *parent)
        : QObject(parent)
        , discardBytes(discardBytes)
{
    worker = new TransferWorker(this, discardBytes);
}

TransferContext::~TransferContext() {
    worker->quit();
    delete worker;
    for(auto strategy : transferStrategies) {
        delete strategy;
    }
}

void TransferContext::transfer(const IoData &data) {
    worker->transfer(data);
}

void TransferContext::setReadCacheSize(int size) {
    if(transferStrategies.contains(curTransStrategy)) {
        transferStrategies[curTransStrategy]->setReadCacheSize(size);
    } else {
        worker->setReadCacheSize(size);
    }
}

TransferWorker::TransferWorker(TransferContext* context, uint8_t discardBytes, QObject *parent)
    : QObject(parent)
    , context(context)
    , discardBytes(discardBytes)
{
    thr = new QThread;
    connect(this, &TransferWorker::transfer, this, &TransferWorker::executeTransfer);
    this->moveToThread(thr);
    thr->start();
}

void TransferWorker::executeTransfer(const IoData &data) {
    if(!context->transferStrategies.contains(data.transferStrategy)) {
        makeStrategy(data.transferStrategy);
    } else if(context->curTransStrategy != data.transferStrategy) {
        context->curTransStrategy = data.transferStrategy;
    }
    context->transferStrategies[data.transferStrategy]->transfer(data);
}

void TransferWorker::makeStrategy(TransferStrategy strategy) {
    StrategyBase* transStrategy;
    switch (strategy) {
        case TransferStrategy::SYNC_BULK: transStrategy = new SyncBulkTransfer; break;
        case TransferStrategy::SYNC_CONTROL: transStrategy = new SyncControlTransfer; break;
        case TransferStrategy::SYNC_INTERRUPT: transStrategy = new SyncInterTransfer(discardBytes); break;
        case TransferStrategy::ASYNC_BULK: assert(false);
        case TransferStrategy::ASYNC_ISO: assert(false);
        case TransferStrategy::ASYNC_CONTROL: assert(false);
        case TransferStrategy::ASYNC_INTERRUPT: assert(false);
    }
    context->transferStrategies[strategy] = transStrategy;
    connect(transStrategy, &StrategyBase::transferFinished, context, &TransferContext::transferFinished);
    transStrategy->setReadCacheSize(readCacheSize);
}

void TransferWorker::quit() {
    thr->quit();
    thr->wait();
}

TransferWorker::~TransferWorker() {
    qCDebug(usbCategory) << "transfer worker release!";
}

void TransferWorker::setReadCacheSize(int size) {
    readCacheSize = size;
}

QT_USB_NAMESPACE_END
