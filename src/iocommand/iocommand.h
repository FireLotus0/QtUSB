#pragma once

#include "QtUsb/usb_namespace.h"
#include "QtUsb/libusb.h"
#include "transfer/transfercontext.h"
#include "descriptor/descriptorbase/descriptordata.h"
#include <qobject.h>
#include <qtimer.h>
#include <QQueue>

QT_USB_NAMESPACE_BEGIN

class IoCommand : public QObject {
Q_OBJECT
public:
    explicit IoCommand(const DescriptorData& descriptorData, libusb_device_handle* handle, QObject *parent = nullptr);

    ~IoCommand();

    void setConfiguration(const ActiveUSBConfig& cfg);

    void read();

    void write(QByteArray &&data);

    void setSpeedPrintEnable(bool enable);

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

    void initSpeedTimer();

    void releaseContext();

private:
    struct IoContext {
        TransferContext* readContext{nullptr}, *writeContext{nullptr}, *transferContext{nullptr};
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

    bool speedPrintable = false;
    QTimer speedPrintTimer;
    quint64 bytesCounter = 0;
    const quint64 bytesMB = 1024 * 1024;
    QString speedUnit = "mb/s";
};

QT_USB_NAMESPACE_END