#include "syncintertransfer.h"
#include <qthread.h>

QT_USB_NAMESPACE_BEGIN

SyncInterTransfer::SyncInterTransfer(uint8_t discardBytes, QObject *parent)
        : StrategyBase(discardBytes, parent)
{
}

void SyncInterTransfer::transfer(const IoData &request) {
    int transferred = 0;
    auto result = request;
    if(request.transferDirection == TransferDirection::HOST_TO_DEVICE) {
        int dataSize = request.data.size();
        int totalTransferred = 0;
        int expectWrite;
        while(transferred < dataSize) {
            expectWrite = dataSize - totalTransferred > request.maxPacketSize ?  request.maxPacketSize : dataSize - totalTransferred;
            result.resultCode = libusb_interrupt_transfer(request.handle, request.address,
                                                     (unsigned char*)request.data.data() + totalTransferred, expectWrite, &transferred, timeout);
            if(result.resultCode  != LIBUSB_SUCCESS) {
                emit transferFinished(result);
                return;
            }
            totalTransferred += transferred;
            if(transferInterval > 0) {
                QThread::msleep(transferInterval);
            }
        }
        emit transferFinished(result);
    } else {
        adjustReadCacheSz(request.maxPacketSize);
        readCache.fill(0);
        result.resultCode = libusb_interrupt_transfer(request.handle,  request.address, (unsigned char*)readCache.data(), readCacheSize, &transferred, timeout);
        if(result.resultCode == LIBUSB_SUCCESS) {
            result.data = QByteArray::fromRawData(readCache.data(), transferred);
        }
        emit transferFinished(result);
    }
}

QT_USB_NAMESPACE_END