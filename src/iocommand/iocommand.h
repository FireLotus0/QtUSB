#pragma once

#include "QtUsb/usb_namespace.h"
#include "QtUsb/libusb.h"
#include "transfer/transfercontext.h"
#include "descriptor/descriptorbase/descriptordata.h"
#include "utils/funcdelegate.h"
#include <qobject.h>
#include <qtimer.h>
#include <QQueue>

QT_USB_NAMESPACE_BEGIN
class UsbDevice;

class IoCommand : public QObject {
    Q_OBJECT

public:
    explicit IoCommand(const DescriptorData &descriptorData, libusb_device_handle *handle, UsbDevice *device, QObject *parent = nullptr);

    ~IoCommand();

    void setConfiguration(const ActiveUSBConfig &cfg);

    void read();

    void write(QByteArray &&data);

    void write(const QByteArray &data);

    void setSpeedPrintEnable(bool readSpeed, bool writtenSpeed);

signals:
    void transferFinished(TransferDirection direction, int transferred);

private slots:
    void onTransferFinished(TransferDirection direction, int transferred);

private:
    void initContext();

    template<typename T>
    void makeIoData(TransferDirection direction, T &&data);

    void doTransfer(bool read);

    void initSpeedTimer();

    void releaseContext();

    bool checkDevValid(bool isRead) const;

    void printSpeed(bool isWriteSpeed);

private:
    struct IoContext {
        TransferContext *readContext{nullptr}, *writeContext{nullptr}, *transferContext{nullptr};
        QQueue<IoData> readQueue, writeQueue, transferQueue;
        bool isReading{false}, isWriting{false}, isTransferring{false};
    };

private:
    DescriptorData descriptorData;
    ConfigurationData *curCfg{nullptr};
    InterfaceData *curInterface{nullptr};

    ActiveUSBConfig config;
    IoContext ioContext;
    libusb_device_handle *handle;

    bool writeSpeedPrintable = false;
    bool readSpeedPrintable = false;
    QTimer speedPrintTimer;

    int bytesWritten = 0, bytesRead = 0;
    const double bytesMB = 1024 * 1024;
    QString speedUnit = "mb/s";

    EventDelegate eventDelegate;
};

QT_USB_NAMESPACE_END
