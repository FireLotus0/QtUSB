#pragma once

#include "src/transfer/impl/strategybase.h"
#include <qobject.h>

QT_USB_NAMESPACE_BEGIN

class SyncBulkTransfer : public StrategyBase {
public:
    explicit SyncBulkTransfer(QObject *parent = nullptr);

    void transfer(const IoData &request) override;
};

QT_USB_NAMESPACE_END