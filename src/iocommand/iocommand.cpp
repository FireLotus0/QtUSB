#include "iocommand.h"

#include <qloggingcategory.h>

QT_USB_NAMESPACE_BEGIN

const QLoggingCategory &usbCategory();

IoCommand::IoCommand(const QT_USB::DescriptorData &descriptorData, libusb_device_handle *handle, QObject *parent)
        : descriptorData(descriptorData), handle(handle), QObject(parent) {
    initSpeedTimer();
}

void IoCommand::setConfiguration(const ActiveUSBConfig &cfg) {
    config = cfg;
    if (!descriptorData.configurations.contains(config.configuration)) {
        qCWarning(usbCategory) << "Invalid configuration: " << config.configuration << " support configurations: "
                               << descriptorData.configurations.keys();
        return;
    } else {
        curCfg = &descriptorData.configurations[config.configuration];
    }
    if (!curCfg->interfaces.contains(config.interface)) {
        qCWarning(usbCategory) << "Invalid interface: " << config.interface << " support interfaces: "
                               << curCfg->interfaces.keys();
        return;
    } else {
        curInterface = &curCfg->interfaces[config.interface];
    }
    initContext();
}

void IoCommand::initContext() {
    releaseContext();
    // 半双工并且需要对命令进行排队处理，否则即使是半双工模式，写命令的优先级高于读命令
    if (!descriptorData.fullDuplexSupported && config.queuedCommands) {
        ioContext.transferContext = new TransferContext(config.discardBytes, config.cmdInterval);
        connect(ioContext.transferContext, &TransferContext::transferFinished, this,
                &IoCommand::onTransferFinished);
    } else {
        ioContext.readContext = new TransferContext(config.discardBytes, config.cmdInterval);
        connect(ioContext.readContext, &TransferContext::transferFinished, this, &IoCommand::onTransferFinished);

        if (!descriptorData.fullDuplexSupported) {
            // 半双工模式下，实现写命令优先于读取命令
            ioContext.writeContext = ioContext.readContext;
        } else {
            // 全双工模式
            ioContext.writeContext = new TransferContext(config.discardBytes, config.cmdInterval);
            connect(ioContext.writeContext, &TransferContext::transferFinished, this,
                    &IoCommand::onTransferFinished);
        }
    }
    // 全双工模式或者半双工并且需要写优先级高于读
    if (!config.queuedCommands) {
        ioContext.readContext->setReadCacheSize(config.readCacheSize);
    } else {
        ioContext.transferContext->setReadCacheSize(config.readCacheSize);
    }
}

IoCommand::~IoCommand() {
    releaseContext();
}

void IoCommand::read() {
    if(checkDevValid(true)) {
        makeIoData(TransferDirection::DEVICE_TO_HOST, QByteArray{});
        doTransfer(true);
    }
}

void IoCommand::write(QByteArray &&data) {
    if(checkDevValid(false)) {
        makeIoData(TransferDirection::HOST_TO_DEVICE, std::move(data));
        doTransfer(false);
    }
}

void IoCommand::write(const QByteArray &data) {
    if(checkDevValid(false)) {
        makeIoData(TransferDirection::HOST_TO_DEVICE, data);
        doTransfer(false);
    }
}


void IoCommand::doTransfer(bool read) {
    if (read) {
        if ((descriptorData.fullDuplexSupported || !config.queuedCommands) && !ioContext.isReading) {
            if (ioContext.readQueue.empty()) {
                return;
            }
            ioContext.readContext->transfer(ioContext.readQueue.head());
            ioContext.isReading = true;
        } else if ((!descriptorData.fullDuplexSupported || config.queuedCommands) && !ioContext.isTransferring) {
            if (ioContext.transferQueue.empty()) {
                return;
            }
            ioContext.transferContext->transfer(ioContext.transferQueue.head());
            ioContext.isTransferring = true;
        }
    } else {
        if ((descriptorData.fullDuplexSupported || !config.queuedCommands) && !ioContext.isWriting) {
            if (ioContext.writeQueue.empty()) {
                return;
            }
            ioContext.writeContext->transfer(ioContext.writeQueue.head());
            ioContext.isWriting = true;
        } else if ((!descriptorData.fullDuplexSupported || config.queuedCommands) && !ioContext.isTransferring) {
            if (ioContext.transferQueue.empty()) {
                return;
            }
            ioContext.transferContext->transfer(ioContext.transferQueue.head());
            ioContext.isTransferring = true;
        }
    }
}

void IoCommand::onTransferFinished(const IoData &data) {
    if (speedPrintable) {
        bytesCounter += data.data.size();
    }
    if (data.transferDirection == TransferDirection::DEVICE_TO_HOST) {
        if (descriptorData.fullDuplexSupported || !config.queuedCommands) {
            ioContext.readQueue.dequeue();
            ioContext.isReading = false;
        } else {
            ioContext.transferQueue.dequeue();
            ioContext.isTransferring = false;
        }
        if (data.resultCode == LIBUSB_SUCCESS) {
            emit readFinished(data.data);
        }
    } else {
        if (descriptorData.fullDuplexSupported || !config.queuedCommands) {
            ioContext.writeQueue.dequeue();
            ioContext.isWriting = false;
        } else {
            ioContext.transferQueue.dequeue();
            ioContext.isTransferring = false;
        }
        if (data.resultCode == LIBUSB_SUCCESS) {
            emit writeFinished();
        }
    }
    if (config.queuedCommands) {
        doTransfer(data.transferDirection == TransferDirection::DEVICE_TO_HOST);
    } else {
        if (ioContext.writeQueue.empty()) {
            doTransfer(data.transferDirection == TransferDirection::DEVICE_TO_HOST);
        } else {
            // 优先传输写命令
            doTransfer(false);
        }
    }
    if (data.resultCode != LIBUSB_SUCCESS) {
        emit errorOccurred(data.resultCode, QString(libusb_error_name(data.resultCode)));
    }
}

void IoCommand::setSpeedPrintEnable(bool enable) {
    speedPrintable = enable;
    if (enable) {
        speedPrintTimer.start();
    } else {
        speedPrintTimer.stop();
    }
}

void IoCommand::initSpeedTimer() {
    speedPrintTimer.setInterval(1000);
    speedPrintTimer.callOnTimeout([&] {
        double speed = 0.0;
        if (bytesCounter >= bytesMB) {
            speed = (bytesCounter * 1.0) / bytesMB;
            speedUnit = "mb/s";
        } else {
            speed = (bytesCounter * 1.0) / 1024;
            speedUnit = "kb/s";
        }
        qCInfo(usbCategory).noquote() << "read speed: " << QString::number(speed, 'f', 3) << speedUnit;
        bytesCounter = 0;
    });
}

void IoCommand::releaseContext() {
    if (ioContext.transferContext) {
        delete ioContext.transferContext;
        ioContext.transferContext = nullptr;
    }
    if (ioContext.readContext) {
        delete ioContext.readContext;
        ioContext.readContext = nullptr;
    }
    if (descriptorData.fullDuplexSupported) {
        delete ioContext.writeContext;
    }
    ioContext.writeContext = nullptr;
}

bool IoCommand::checkDevValid(bool isRead) const {
    if (!curCfg || !curInterface) {
        qCWarning(usbCategory) << QString("%1 Failed: Invalid Active Usb Configuration: configuration valid = ").arg(isRead ? "Read" : "Write")
                               << (curCfg != nullptr) << " interface valid = " << (curInterface != nullptr);
        return false;
    }
    return true;
}

template<typename T>
void IoCommand::makeIoData(TransferDirection direction, T &&data) {
    IoData ioData;
    ioData.handle = handle;
    ioData.address = config.pointNumber |
                     (direction == TransferDirection::HOST_TO_DEVICE ? LIBUSB_ENDPOINT_OUT : LIBUSB_ENDPOINT_IN);
    if (!curInterface->endpoints.contains(ioData.address)) {
        qCWarning(usbCategory) << "Invalid point address: " << ioData.address << " point number:"
                               << config.pointNumber << " support point addresses: "
                               << curInterface->endpoints.keys();
        return;
    }
    auto &endPoint = curInterface->endpoints[ioData.address];

    ioData.maxPacketSize = endPoint.maxPacketSize;
    ioData.transferDirection = direction;
    ioData.transferStrategy = transTypeToStrategy(true, endPoint.transferType);
    if constexpr (std::is_rvalue_reference_v<decltype(data)>) {
        ioData.data =  std::move(data);
    } else {
        ioData.data = data;
    }
    if (config.queuedCommands) {
        ioContext.transferQueue.enqueue(ioData);
    } else {
        if (direction == TransferDirection::HOST_TO_DEVICE) {
            ioContext.writeQueue.enqueue(ioData);
        } else {
            ioContext.readQueue.enqueue(ioData);
        }
    }
}


QT_USB_NAMESPACE_END


