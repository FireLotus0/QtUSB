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
        readUsbTimer.setInterval(1);
        readUsbTimer.callOnTimeout([&] {
            device->read();
        });
    }

public slots:
    void onDeviceAttached(UsbId id) {
        device = UsbDevManager::instance().getDevice(id);
        device->printInfo();
        ActiveUSBConfig config;
        config.interface = 0;
        config.configuration = 0;
        config.pointNumber = 1;
        config.readCacheSize = 1024 * 60;
        device->setConfiguration(config);

        // 开启速度打印
        device->setSpeedPrintEnable(true);
        readUsbTimer.start();
    }

    void onDeviceDetached(UsbId id) {
        qDebug() << "Device detached";
        // readUsbTimer.stop();
        // device.reset();
    }

private:
    void initUsbSig() {
//         connect(device.get(), &UsbDevice::readFinished, this, [&](const QByteArray &data) {
//             qDebug() << "read data: " << data.toHex(' ');
//         });
//         connect(device.get(), &UsbDevice::errorOccurred, this, [&](int errorCode, const QString &errorString) {
//             qDebug() << "usb error: " << errorString;
//         });
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
        UsbDevManager::instance().addMonitorId({0x010c, 0x0d7d});
        UsbDevManager::instance().addMonitorId({0x4831, 0x4831});
//        UsbDevManager::instance().addMonitorClass(DeviceType::ANY_CLASS);
    });
    QObject::connect(&application, &QCoreApplication::aboutToQuit, [&]() {
        UsbDevManager::instance().deleteLater();
    });
    application.exec();
}

#include "main.moc"
