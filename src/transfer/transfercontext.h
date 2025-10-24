#pragma once

#include "src/usb_namespace.h"
#include "src/transfer/impl/strategybase.h"
#include <qobject.h>
#include <qmap.h>

QT_USB_NAMESPACE_BEGIN

class TransferWorker;

class TransferContext : public QObject {
Q_OBJECT
public:
    explicit TransferContext(QObject *parent = nullptr);

    ~TransferContext();

    void transfer(const IoData& data);

signals:
    void transferFinished(const IoData& response);

private:
    TransferStrategy curTransStrategy{};
    QMap<TransferStrategy, StrategyBase *> transferStrategies;

    friend class TransferWorker;

    TransferWorker *worker;
};

class TransferWorker : public QObject {
Q_OBJECT
public:
    TransferWorker(TransferContext *context, QObject *parent = nullptr);

    ~TransferWorker();

    void quit();

signals:
    void transfer(const IoData& data);

private slots:
    void executeTransfer(const IoData& data);

private:
    void makeStrategy(TransferStrategy strategy);

private:
    TransferContext *context;
    QThread *thr;
};

QT_USB_NAMESPACE_END
