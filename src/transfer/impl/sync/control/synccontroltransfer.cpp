#include "synccontroltransfer.h"
#include <qthread.h>

QT_USB_NAMESPACE_BEGIN

SyncControlTransfer::SyncControlTransfer(int timeout, EventDelegate* eventDelegate, QObject *parent)
        : StrategyBase(0, 0, timeout, eventDelegate, parent) {
}

void SyncControlTransfer::transfer(const IoData &request) {
    auto result = request;
    result.resultCode = libusb_control_transfer(request.handle, request.controlRequestData.requestType, request.controlRequestData.request,
                                                request.controlRequestData.value, request.controlRequestData.index, (unsigned char*)request.data.data(),
                                                request.controlRequestData.length, timeout);
    TransferDirection direction = (request.controlRequestData.requestType & LIBUSB_ENDPOINT_IN) ? TransferDirection::DEVICE_TO_HOST : TransferDirection::HOST_TO_DEVICE;
    if (result.resultCode != LIBUSB_SUCCESS) {
        eventDelegate->errorOccurredDelegate(result.resultCode, QString(libusb_error_name(result.resultCode)));
    } else {
        if (direction == TransferDirection::DEVICE_TO_HOST) {
            eventDelegate->readFinishedDelegate(request.data);
        } else {
            eventDelegate->writeFinishedDelegate();
        }
    }
    eventDelegate->transferFinishDelegate(direction, request.controlRequestData.length);
}

QT_USB_NAMESPACE_END