#pragma once

#include "include/QtUsb/usb_namespace.h"
#include "../../include/QtUsb/libusb.h"
#include "src/transfer/transfercontext.h"
#include "src/descriptor/descriptorbase/descriptordata.h"
#include <qobject.h>
#include <qqueue>

QT_USB_NAMESPACE_BEGIN

class IoCommand : public QObject {
Q_OBJECT
public:
    explicit IoCommand(const DescriptorData& descriptorData, libusb_device_handle* handle, QObject *parent = nullptr);

    ~IoCommand();

    void setConfiguration(const ActiveUSBConfig& cfg);

    void read();

    void write(QByteArray &&data);

signals:
    void readFinished(const QByteArray& data);
    void writeFinished();
    void errorOccurred(int errorCode, const QString& errorString);

private slots:
    void onTransferFinished(const IoData& data);

private:
    void initContext();

    void makeIoData(TransferDirection direction, QByteArray&& data);

    void doTransfer(bool read);

private:
    struct IoContext {
        TransferContext* readContext, *writeContext, *transferContext;
        QQueue<IoData> readQueue, writeQueue, transferQueue;
        bool isReading{false}, isWriting{false}, isTransferring{false};
    };

private:
    DescriptorData descriptorData;
    ConfigurationData* curCfg;
    InterfaceData* curInterface;

    ActiveUSBConfig config;
    IoContext ioContext;
    libusb_device_handle* handle;
};

QT_USB_NAMESPACE_END