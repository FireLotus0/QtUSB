#pragma once

#include "QtUsb/datatypes.h"
#include <qobject.h>

QT_USB_NAMESPACE_BEGIN

class StrategyBase : public QObject {
Q_OBJECT
public:
    explicit StrategyBase(uint8_t discardBytes, uint8_t cmdInterval, QObject *parent = nullptr);

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
    uint8_t cmdInterval = 0;
    QByteArray readCache;
    uint8_t discardBytes; // 用于在Windows平台上使用单片机进行中断传输时，分包处理时使用
};

QT_USB_NAMESPACE_END