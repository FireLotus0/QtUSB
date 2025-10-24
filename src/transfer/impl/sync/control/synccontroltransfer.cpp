#include "synccontroltransfer.h"
#include <qthread.h>

QT_USB_NAMESPACE_BEGIN

SyncControlTransfer::SyncControlTransfer(QObject *parent)
        : StrategyBase(parent) {
}

void SyncControlTransfer::transfer(const IoData &request) {
    int transferred = 0;
    auto result = request;
    result.resultCode = libusb_control_transfer(request.handle, request.controlRequestData.requestType, request.controlRequestData.request,
                                                request.controlRequestData.value, request.controlRequestData.index, (unsigned char*)request.data.data(),
                                                request.controlRequestData.length, timeout);
    emit transferFinished(result);
}

QT_USB_NAMESPACE_END