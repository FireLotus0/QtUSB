#include "strategybase.h"

QT_USB_NAMESPACE_BEGIN

StrategyBase::StrategyBase(QObject *parent)
        : QObject(parent) {
}

void StrategyBase::setReadCacheSize(int size) {
    readCacheSize.storeRelease(size);
}

QT_USB_NAMESPACE_END