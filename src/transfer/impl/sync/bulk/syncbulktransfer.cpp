#include "syncbulktransfer.h"
#include <qthread.h>

QT_USB_NAMESPACE_BEGIN

SyncBulkTransfer::SyncBulkTransfer(QObject *parent)
        : StrategyBase(parent)
{
}

void SyncBulkTransfer::transfer(const IoData &request) {
    int transferred = 0;
    auto result = request;
    if(request.transferDirection == TransferDirection::HOST_TO_DEVICE) {
        result.data.clear();
        int dataSize = request.data.size();
        int totalTransferred = 0;
        int expectWrite = 0;
        while(transferred < dataSize) {
            expectWrite = dataSize - totalTransferred > request.maxPacketSize ?  request.maxPacketSize : dataSize - totalTransferred;
            result.resultCode = libusb_bulk_transfer(request.handle, request.address,
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
    } else {
        adjustReadCacheSz(request.maxPacketSize);
        readCache.fill(0);
        result.resultCode = libusb_bulk_transfer(request.handle,  request.address, (unsigned char*)readCache.data(), readCacheSize, &transferred, timeout);
        if(result.resultCode == LIBUSB_SUCCESS) {
            result.data = QByteArray::fromRawData(readCache.data(), transferred);
        }
        emit transferFinished(result);
    }
}

QT_USB_NAMESPACE_END