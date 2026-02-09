#pragma once

#include "transfer/impl/strategybase.h"
#include <qobject.h>

QT_USB_NAMESPACE_BEGIN

class SyncBulkTransfer : public StrategyBase {
public:
    explicit SyncBulkTransfer(uint8_t cmdInterval, int timeout, EventDelegate* eventDelegate, QObject *parent = nullptr);

    void transfer(const IoData &request) override;
};

QT_USB_NAMESPACE_END