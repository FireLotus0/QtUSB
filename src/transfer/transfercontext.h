#pragma once

#include "QtUsb/usb_namespace.h"
#include "transfer/impl/strategybase.h"
#include <qobject.h>
#include <qmap.h>

QT_USB_NAMESPACE_BEGIN

class TransferWorker;

class TransferContext : public QObject {
Q_OBJECT
public:
    explicit TransferContext(uint8_t discardBytes, QObject *parent = nullptr);

    ~TransferContext();

    void transfer(const IoData& data);

    void setReadCacheSize(int size);

signals:
    void transferFinished(const IoData& response);

private:
    TransferStrategy curTransStrategy{};
    QMap<TransferStrategy, StrategyBase *> transferStrategies;
    friend class TransferWorker;
    TransferWorker *worker;
    uint8_t discardBytes = 0;   // 用于在Windows平台上使用单片机进行中断传输时，分包处理时使用
};

class TransferWorker : public QObject {
Q_OBJECT
public:
    TransferWorker(TransferContext *context, uint8_t discardBytes, QObject *parent = nullptr);

    ~TransferWorker();

    void quit();

    void setReadCacheSize(int size);
signals:
    void transfer(const IoData& data);

private slots:
    void executeTransfer(const IoData& data);

public:
    void makeStrategy(TransferStrategy strategy);

private:
    TransferContext *context;
    QThread *thr;
    int readCacheSize = 0ULL;
    uint8_t discardBytes;
};

QT_USB_NAMESPACE_END
