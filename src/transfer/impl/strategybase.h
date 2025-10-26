#pragma once

#include "../../../include/QtUsb/datatypes.h"
#include <qobject.h>

QT_USB_NAMESPACE_BEGIN

class StrategyBase : public QObject {
Q_OBJECT
public:
    explicit StrategyBase(QObject *parent = nullptr);

    virtual void transfer(const IoData &request) = 0;

signals:
    void transferFinished(const IoData& rsponse);

protected:
    int readCacheSize = 1024;
    int timeout = 2000;
    int transferInterval = 0;
    QByteArray readCache;
};

QT_USB_NAMESPACE_END