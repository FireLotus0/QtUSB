#include "syncbulktransfer.h"
#include "QtUsb/usbdevice.h"
#include <qthread.h>


QT_USB_NAMESPACE_BEGIN

class UsbDevice;
const QLoggingCategory &usbCategory();

SyncBulkTransfer::SyncBulkTransfer(uint8_t cmdInterval, int timeout, EventDelegate* eventDelegate, QObject *parent)
        : StrategyBase(0, cmdInterval, timeout, eventDelegate, parent)
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
        while(totalTransferred < dataSize) {
            expectWrite = dataSize - totalTransferred > request.maxPacketSize ?  request.maxPacketSize : dataSize - totalTransferred;
            result.resultCode = libusb_bulk_transfer(request.handle, request.address,
                                           (unsigned char*)request.data.data() + totalTransferred, expectWrite, &transferred, timeout);
            if(result.resultCode != LIBUSB_SUCCESS) {
                eventDelegate->errorOccurredDelegate(result.resultCode, QString(libusb_error_name(result.resultCode)));
                eventDelegate->transferFinishDelegate(request.transferDirection, 0);
                return;
            }
            totalTransferred += transferred;
            if(cmdInterval > 0) {
                QThread::msleep(cmdInterval);
            }
        }
        eventDelegate->writeFinishedDelegate();
        eventDelegate->transferFinishDelegate(request.transferDirection, totalTransferred);
    } else {
        adjustReadCacheSz(request.maxPacketSize);
        readCache.fill(0);
        result.resultCode = libusb_bulk_transfer(request.handle,  request.address, (unsigned char*)readCache.data(), readCacheSize, &transferred, timeout);

        if (result.resultCode == LIBUSB_SUCCESS) {
            result.data = QByteArray::fromRawData(readCache.data(), transferred);
            eventDelegate->readFinishedDelegate(result.data);
        } else {
            transferred = 0;
            eventDelegate->errorOccurredDelegate(result.resultCode, QString(libusb_error_name(result.resultCode)));
        }
        eventDelegate->transferFinishDelegate(result.transferDirection, transferred);
    }
}

QT_USB_NAMESPACE_END