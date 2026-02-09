#pragma once
#include "transfer/impl/strategybase.h"
#include <qobject.h>

QT_USB_NAMESPACE_BEGIN

class SyncInterTransfer : public StrategyBase {
public:
    explicit SyncInterTransfer(uint8_t discardBytes, uint8_t cmdInterval, int timeout, EventDelegate* eventDelegate, QObject *parent = nullptr);

    void transfer(const IoData &request) override;
};

QT_USB_NAMESPACE_END