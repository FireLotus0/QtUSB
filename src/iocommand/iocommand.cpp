#include "iocommand.h"
#include <qdebug.h>

QT_USB_NAMESPACE_BEGIN

IoCommand::IoCommand(const QT_USB::DescriptorData &descriptorData, libusb_device_handle* handle, QObject *parent)
    : descriptorData(descriptorData)
    , handle(handle)
    , QObject(parent)
{
    initContext();
    initSpeedTimer();
}

void IoCommand::setConfiguration(const ActiveUSBConfig &cfg) {
    config = cfg;
    if(!descriptorData.configurations.contains(config.configuration)) {
        qWarning() << "Invalid configuration: " << config.configuration << " support configurations: " << descriptorData.configurations.keys();
        return;
    } else {
        curCfg =  &descriptorData.configurations[config.configuration];
    }
    if(!curCfg->interfaces.contains(config.interface)) {
        qWarning() << "Invalid interface: " << config.interface << " support interfaces: " << curCfg->interfaces.keys();
        return;
    } else {
        curInterface = &curCfg->interfaces[config.interface];
    }
    if(descriptorData.fullDuplexSupported) {
        ioContext.readContext->setReadCacheSize(cfg.readCacheSize);
    } else {
        ioContext.transferContext->setReadCacheSize(cfg.readCacheSize);
    }
}

void IoCommand::initContext() {
    if(!descriptorData.fullDuplexSupported) {
        ioContext.transferContext = new TransferContext;
        connect(ioContext.transferContext, &TransferContext::transferFinished, this, &IoCommand::onTransferFinished);
    } else {
        ioContext.readContext = new TransferContext;
        ioContext.writeContext = new TransferContext;
        connect(ioContext.readContext, &TransferContext::transferFinished, this, &IoCommand::onTransferFinished);
        connect(ioContext.writeContext, &TransferContext::transferFinished, this, &IoCommand::onTransferFinished);
    }
}

IoCommand::~IoCommand() {
    if(!descriptorData.fullDuplexSupported) {
        delete ioContext.transferContext;
    } else {
        delete ioContext.readContext;
        delete ioContext.writeContext;
    }
}

void IoCommand::makeIoData(TransferDirection direction, QByteArray &&data) {
    IoData ioData;
    ioData.handle = handle;
    ioData.address = config.pointNumber | (direction == TransferDirection::HOST_TO_DEVICE ? LIBUSB_ENDPOINT_OUT : LIBUSB_ENDPOINT_IN);
    if(!curInterface->endpoints.contains(ioData.address)) {
        qWarning() << "Invalid point address: " << ioData.address << " point number:" << config.pointNumber << " support point addresses: "
            << curInterface->endpoints.keys();
        return;
    }
    auto& endPoint = curInterface->endpoints[ioData.address];

    ioData.maxPacketSize = endPoint.maxPacketSize;
    ioData.transferDirection = direction;
    ioData.transferStrategy = transTypeToStrategy(true, endPoint.transferType);
    ioData.data = std::move(data);
    if(!descriptorData.fullDuplexSupported) {
        ioContext.transferQueue.enqueue(ioData);
    } else {
        if(direction == TransferDirection::HOST_TO_DEVICE) {
            ioContext.writeQueue.enqueue(ioData);
        } else {
            ioContext.readQueue.enqueue(ioData);
        }
    }
}

void IoCommand::read() {
    assert(curInterface);
    makeIoData(TransferDirection::DEVICE_TO_HOST, {});
    doTransfer(true);
}

void IoCommand::write(QByteArray &&data) {
    assert(curInterface);
    makeIoData(TransferDirection::HOST_TO_DEVICE, std::move(data));
    doTransfer(false);
}

void IoCommand::doTransfer(bool read) {
    if(read) {
        if(descriptorData.fullDuplexSupported && !ioContext.isReading) {
            if(ioContext.readQueue.empty()) {
                return;
            }
            ioContext.readContext->transfer(ioContext.readQueue.head());
            ioContext.isReading = true;
        } else if(!descriptorData.fullDuplexSupported && !ioContext.isTransferring){
            if(ioContext.transferQueue.empty()) {
                return;
            }
            ioContext.transferContext->transfer(ioContext.transferQueue.head());
            ioContext.isTransferring = true;
        }
    } else {
        if(descriptorData.fullDuplexSupported && !ioContext.isWriting) {
            if(ioContext.writeQueue.empty()) {
                return;
            }
            ioContext.writeContext->transfer(ioContext.writeQueue.head());
            ioContext.isWriting = true;
        } else if(!descriptorData.fullDuplexSupported && !ioContext.isTransferring) {
            if(ioContext.transferQueue.empty()) {
                return;
            }
            ioContext.transferContext->transfer(ioContext.transferQueue.head());
            ioContext.isTransferring = true;
        }
    }
}

void IoCommand::onTransferFinished(const IoData &data) {
    if(speedPrintable) {
        bytesCounter += data.data.size();
    }
    if(data.transferDirection == TransferDirection::DEVICE_TO_HOST) {
        if(descriptorData.fullDuplexSupported) {
            ioContext.readQueue.dequeue();
            ioContext.isReading = false;
        } else {
            ioContext.transferQueue.dequeue();
            ioContext.isTransferring = false;
        }
        if(data.resultCode == LIBUSB_SUCCESS) {
            emit readFinished(data.data);
        }
        doTransfer(true);
    } else {
        if(descriptorData.fullDuplexSupported) {
            ioContext.writeQueue.dequeue();
            ioContext.isWriting = false;
        } else {
            ioContext.transferQueue.dequeue();
            ioContext.isTransferring = false;
        }
        if(data.resultCode == LIBUSB_SUCCESS) {
            emit writeFinished();
        }
        doTransfer(false);
    }
    if(data.resultCode != LIBUSB_SUCCESS) {
        emit errorOccurred(data.resultCode, QString(libusb_error_name(data.resultCode)));
    }
}

void IoCommand::setSpeedPrintEnable(bool enable) {
    speedPrintable = enable;
    if(enable) {
        speedPrintTimer.start();
    } else {
        speedPrintTimer.stop();
    }
}

void IoCommand::initSpeedTimer() {
    speedPrintTimer.setInterval(1000);
    speedPrintTimer.callOnTimeout([&]{
        double speed = 0.0;
        if(bytesCounter >= bytesMB) {
            speed = (bytesCounter * 1.0) / bytesMB;
            speedUnit = "mb/s";
        } else {
            speed = (bytesCounter * 1.0) / 1024;
            speedUnit = "kb/s";
        }
        qInfo().noquote() << "read speed: " << QString::number(speed, 'f', 3) << speedUnit;
        bytesCounter = 0;
    });
}

QT_USB_NAMESPACE_END


