#include "../include/QtUsb/usb_namespace.h"
#include "../include/QtUsb/usbdevmanager.h"
#include "../include/QtUsb/usbdevice.h"
#include <qdebug.h>
#include <qcoreapplication.h>
#include <qtimer.h>

USING_QT_USB_NAMESPACE

class UsbUser : public QObject {
    Q_OBJECT

public:
    explicit UsbUser(QObject *parent = 0) : QObject(parent) {
        readUsbTimer.setInterval(100);
        readUsbTimer.callOnTimeout([&] {
            device->read();
        });
    }

public slots:
    void onDeviceAttached(UsbId id) {
        qDebug() << "Device attached";
        // device = UsbDevManager::instance().getDevice(id);
        // if (device) {
        //     initUsbSig();
        //     device->setConfiguration({1, 0, 1, 8});
        //     device->setSpeedPrintEnable(true);
        //     readUsbTimer.start();
        // }
    }

    void onDeviceDetached(UsbId id) {
        qDebug() << "Device detached";
        // readUsbTimer.stop();
        // device.reset();
    }

private:
    void initUsbSig() {
        // connect(device.get(), &UsbDevice::readFinished, this, [&](const QByteArray &data) {
        //     qDebug() << "read data: " << data.toHex(' ');
        // });
        // connect(device.get(), &UsbDevice::errorOccurred, this, [&](int errorCode, const QString &errorString) {
        //     qDebug() << "usb error: " << errorString;
        // });
    }

private:
    QSharedPointer<UsbDevice> device;
    QTimer readUsbTimer;
};

int main(int argc, char *argv[]) {
    QCoreApplication application(argc, argv);

    UsbDevManager::instance().setLogLevel(QT_USB::UsbLogLevel::DEBUG);
    UsbUser user;

    QObject::connect(&UsbDevManager::instance(), &UsbDevManager::deviceAttached, &user, &UsbUser::onDeviceAttached);
    QObject::connect(&UsbDevManager::instance(), &UsbDevManager::deviceDetached, &user, &UsbUser::onDeviceDetached);

    QTimer::singleShot(100, [&]() {
//        UsbDevManager::instance().addMonitorId({0x4831, 0x4831});
        UsbDevManager::instance().addMonitorClass(DeviceType::USB_CLASS_MASS_STORAGE);
    });

    application.exec();
}

#include "main.moc"
