#pragma once

#include "transfer/impl/strategybase.h"
#include <qobject.h>

QT_USB_NAMESPACE_BEGIN

class SyncControlTransfer : public StrategyBase {
public:
    explicit SyncControlTransfer(int timeout, EventDelegate* eventDelegate, QObject *parent = nullptr);

    void transfer(const IoData &request) override;
};

QT_USB_NAMESPACE_END