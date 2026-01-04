#include "syncintertransfer.h"
#include <qthread.h>

QT_USB_NAMESPACE_BEGIN

    SyncInterTransfer::SyncInterTransfer(uint8_t discardBytes, uint8_t cmdInterval, QObject *parent)
            : StrategyBase(discardBytes, cmdInterval, parent) {
    }

    void SyncInterTransfer::transfer(const IoData &request) {
        int transferred = 0;
        auto result = request;
        if (request.transferDirection == TransferDirection::HOST_TO_DEVICE) {
            int dataSize = request.data.size();
            int totalTransferred = 0;
            int expectWrite;
            while (totalTransferred < dataSize) {
                expectWrite = dataSize - totalTransferred > request.maxPacketSize ? request.maxPacketSize : dataSize -
                                                                                                            totalTransferred;

                if(expectWrite < request.maxPacketSize) {
                    QByteArray tmp;
                    tmp.resize(request.maxPacketSize);
                    tmp.fill(0);
                    memcpy(tmp.data(), (unsigned char *) request.data.data() + totalTransferred, expectWrite);
                    result.resultCode = libusb_interrupt_transfer(request.handle, request.address,
                                                                  (unsigned char*)tmp.data(), request.maxPacketSize, &transferred, timeout);
                } else {
                    result.resultCode = libusb_interrupt_transfer(request.handle, request.address,
                                                                  (unsigned char*)request.data.data() + totalTransferred, expectWrite, &transferred, timeout);
                }
                if (result.resultCode != LIBUSB_SUCCESS) {
                    emit transferFinished(result);
                    return;
                }
                totalTransferred += (transferred - discardBytes);
                if (cmdInterval > 0) {
                    QThread::msleep(cmdInterval);
                }
            }
            emit transferFinished(result);
        } else {
            if(discardBytes > 0) {
                readCache.resize(request.maxPacketSize + discardBytes);
                readCache.fill(0);
                result.resultCode = libusb_interrupt_transfer(request.handle, request.address,
                                                              (unsigned char *) readCache.data(), readCacheSize,
                                                              &transferred, timeout);
                if (result.resultCode == LIBUSB_SUCCESS) {
                    result.data = QByteArray::fromRawData(readCache.data(), transferred - discardBytes);
                }
                emit transferFinished(result);
            } else {
                adjustReadCacheSz(request.maxPacketSize);
                readCache.fill(0);
                result.resultCode = libusb_interrupt_transfer(request.handle, request.address,
                                                              (unsigned char *) readCache.data(), readCacheSize,
                                                              &transferred, timeout);
                if (result.resultCode == LIBUSB_SUCCESS) {
                    result.data = QByteArray::fromRawData(readCache.data(), transferred);
                }
                emit transferFinished(result);
            }
        }
    }

QT_USB_NAMESPACE_END