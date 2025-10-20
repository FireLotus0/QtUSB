#pragma once

#include "bean/commontype.h"
#include "usbdevice.h"
#include "usblistener.h"

#include <qdebug.h>
#include <qmap.h>
#include <qstring.h>
#include <qthread.h>
#include <qtimer.h>

class UsbManager : public QObject{
    Q_OBJECT
public:
    static UsbManager &instance();

    void appendInterestDev(const QList<QPair<uint16_t, uint16_t >>& dev);

    UsbDevice* getDevice(uint16_t pid, uint16_t vid);
signals:
    void startSearch();

    void connectionChanged(uint16_t pid, uint16_t vid, bool connected);

    void information(const QString& errStr, bool isError);

    void paramsReadFinished();
public slots:
    void onDeviceChanged(const ListenData& devices );
public:
    inline static QMap<QPair<uint16_t , uint16_t >,QPair<libusb_device*, UsbDevice*>> usbDevices{};
    inline static QMap<uint16_t, uint16_t > interestDev;

private:
    UsbManager(QObject* parent = nullptr);

    ~UsbManager();

private:
    bool isRunning = false;
    QVector<QThread> workers;
    QThread listenThread;
};



