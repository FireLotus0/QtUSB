#pragma once
#include "transfer/impl/strategybase.h"
#include <qobject.h>

QT_USB_NAMESPACE_BEGIN

class SyncInterTransfer : public StrategyBase {
public:
    explicit SyncInterTransfer(uint8_t discardBytes, QObject *parent = nullptr);

    void transfer(const IoData &request) override;

private:
    uint8_t discardBytes = 0;
};

QT_USB_NAMESPACE_END