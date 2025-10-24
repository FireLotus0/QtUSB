#include "src/usbmonitor/usbmonitor.h"
#include "src/descriptor/usbdescriptor.h"

#include <qdebug.h>
#include <qcoreapplication.h>
#include <qthread.h>
#include <qtimer.h>

USING_QT_USB_NAMESPACE

class Test : public QObject {
    Q_OBJECT
public:
    Test(QObject *parent = nullptr) : QObject(parent) {}

public slots:
    void onDevAttached(UsbId id) {

    };

    void onDevLeft(UsbId id) {
        qDebug() << "device detached: " << id;
    }
};

int main(int argc, char *argv[]) {
    QCoreApplication application(argc, argv);

    qRegisterMetaType<UsbId>("UsbId");
    qRegisterMetaType<IoData>("RequestData");
    qRegisterMetaType<LibUsbDevWrap>("LibUsbDevWrap*");

    libusb_init_context(NULL, NULL, 0);
    QObject::connect(&UsbMonitor::instance(), &UsbMonitor::deviceAttached, [&](UsbId id, LibUsbDevWrap dev) {
        qDebug() << "device attached: " << id;
        UsbDescriptor descriptor(dev.device);
        descriptor.printInfo();
    });

    QObject::connect(&UsbMonitor::instance(), &UsbMonitor::deviceDetached, [&](UsbId id) {
        qDebug() << "device detached: " << id;
    });

    UsbMonitor::instance().start();
    QTimer timer;
    timer.setInterval(500);
    timer.callOnTimeout([&]{
        UsbMonitor::instance().addMonitorId({0x4831, 0x4831});
    });
    timer.start();
    application.exec();
    libusb_exit(NULL);
}

#include "main.moc"