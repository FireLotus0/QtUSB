#include "strategybase.h"

QT_USB_NAMESPACE_BEGIN

StrategyBase::StrategyBase(QObject *parent)
        : QObject(parent) {
}

void StrategyBase::setReadCacheSize(int size) {
    readCacheSize.storeRelease(size);
}

void StrategyBase::adjustReadCacheSz(int maxPacketSize) {
    if(readCacheSize.loadRelaxed() < maxPacketSize) {
        qCWarning(usbCategory) << QString("readCacheSize(%1) < maxPacketSize(%2), readCacheSize will auto resize to maxPacketSize!")
            .arg(readCacheSize.loadRelaxed()).arg(maxPacketSize);
        readCacheSize.storeRelease(maxPacketSize);
        readCache.resize(maxPacketSize);
    }
    if((readCacheSize.loadRelaxed() % maxPacketSize) != 0) {
        auto newSize = ((readCacheSize.loadRelaxed() - 1) / maxPacketSize + 1) * maxPacketSize;
        qCWarning(usbCategory) << QString("The readCacheSize(%1) is not an integer multiple of the maxPacketSize(%2)."
                                          " readCacheSize will auto resize to %3!")
                    .arg(readCacheSize.loadRelaxed()).arg(maxPacketSize).arg(newSize);
        readCacheSize.storeRelease(newSize);
        readCache.resize(newSize);
    }
}

QT_USB_NAMESPACE_END