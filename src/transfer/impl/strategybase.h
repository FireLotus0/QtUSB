#pragma once

#include "QtUsb/datatypes.h"
#include <qobject.h>

QT_USB_NAMESPACE_BEGIN

class StrategyBase : public QObject {
Q_OBJECT
public:
    explicit StrategyBase(QObject *parent = nullptr);

    virtual void transfer(const IoData &request) = 0;

    void setReadCacheSize(int size);

signals:
    void transferFinished(const IoData& rsponse);

protected:
    // 自动调整读取缓冲区大小为最大包大小的整数倍
    void adjustReadCacheSz(int maxPacketSize);

protected:
    QAtomicInt readCacheSize = 1024;
    int timeout = 2000;
    int transferInterval = 0;
    QByteArray readCache;
};

QT_USB_NAMESPACE_END