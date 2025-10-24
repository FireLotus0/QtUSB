#include "transfercontext.h"
#include "src/transfer/impl/sync/bulk/syncbulktransfer.h"
#include "src/transfer/impl/sync/control/synccontroltransfer.h"
#include "src/transfer/impl/sync/interrupt/syncintertransfer.h"
#include <qthread.h>
#include <qdebug.h>

QT_USB_NAMESPACE_BEGIN

TransferContext::TransferContext(QObject *parent)
        : QObject(parent)
{
    worker = new TransferWorker(this);
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

TransferWorker::TransferWorker(TransferContext* context, QObject *parent)
    : QObject(parent)
    , context(context)
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
        case TransferStrategy::SYNC_INTERRUPT: transStrategy = new SyncInterTransfer; break;
        case TransferStrategy::ASYNC_BULK: assert(false);
        case TransferStrategy::ASYNC_ISO: assert(false);
        case TransferStrategy::ASYNC_CONTROL: assert(false);
        case TransferStrategy::ASYNC_INTERRUPT: assert(false);
    }
    context->transferStrategies[strategy] = transStrategy;
    connect(transStrategy, &StrategyBase::transferFinished, context, &TransferContext::transferFinished);
}

void TransferWorker::quit() {
    thr->quit();
    thr->wait();
}

TransferWorker::~TransferWorker() {
    qDebug() << "transfer worker release!";
}

QT_USB_NAMESPACE_END
