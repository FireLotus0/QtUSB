# QtUsb

基于 Qt 封装的 libusb，旨在简化USB设备读写，方便开发。

---

## 目录

- [功能特点](#功能特点)
- [环境要求](#环境要求)
- [快速开始](#快速开始)
- [类与接口说明](#关键类)
- [示例代码](#示例代码)


---

## 功能特点

- 基于 Qt 的跨平台封装
- 异步传输：libusb同步接口在线程中实现；libusb异步接口目前未实现
- API接口简单，只需要调用read，write进行数据传输，传输完成会发送对应信号
- 支持常见的 USB 设备控制和数据传输
- usb 3.0默认全双工传输，可以通过配置参数设置为半双工，将会对所有读写命令进行队列同步

---

## 环境要求

- Qt5
- libusb 版本（libusb 1.0）
- 支持的操作系统（Windows, Linux）

---

## 快速开始

说明如何集成库：

- 编译依赖(以下为Linux平台，windows使用IDE进行构建即可)
```
git clone git@github.com:FireLotus0/QtUSB.git
cd QtUSB
mkdir build
cd build
cmake -S ../ -B . -DCMAKE_PREFIX_PATH="/opt/Qt/5.15.2/gcc_64;/usr/local/libusb" -DCMAKE_INSTALL_PREFIX=/usr/local/QtUSB
make
make install
```
- 如何将封装库加入你的 Qt 项目
```
find_package(QtUsb REQUIRED)
//...
target_link_libraries (${PROJECT_NAME} PUBLIC QtUsb::QtUsb)
```
- 使用步骤示例
1. 连接设备热插拔信号: UsbDevManager::deviceAttached， UsbDevManager::deviceDetached
2. 添加要监听的设备ID，可监听多个设备：UsbDevManager::instance().addMonitorId({0x4831, 0x4831});
3. 通过ID获取设备: UsbDevManager::instance().getDevice(id);
4. 选择配置： device->setConfiguration(ActiveUSBConfig{1, 0, 1, 8});
5. 监听设备信号: UsbDevice::readFinished，  UsbDevice::writeFinished，  UsbDevice::errorOccurred
6. 读写设备：read， write
---

## 关键类

| 类名          | 说明                     |
|---------------|--------------------------|
| UsbDevice     | USB设备封装类             |
| UsbDevManager | USB设备管理类，提供获取UsbDevice的接口，获取的UsbDevice线程安全           |

---

## 示例代码

```cpp
#include "QtUsb/usbdevmanager.h"
#include "QtUsb/usbdevice.h"
#include <qdebug.h>
#include <qcoreapplication.h>
#include <qthread.h>
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
        QTimer::singleShot(100, [&, id] {
            device = UsbDevManager::instance().getDevice(id);
            if (device) {
                device->setSpeedPrintEnable(true);
                initUsbSig();
                device->setConfiguration({1, 0, 1, 8});
                readUsbTimer.start();
            }
        });
    }

    void onDeviceDetached(UsbId id) {
        QTimer::singleShot(200, [&] {
            readUsbTimer.stop();
            device.reset();
        });
    }

private:
    void initUsbSig() {
        connect(device.get(), &UsbDevice::readFinished, this, [&](const QByteArray &data) {
            qDebug() << "read data: " << data.toHex(' ');
        });
        connect(device.get(), &UsbDevice::errorOccurred, this, [&](int errorCode, const QString &errorString) {
            qDebug() << "usb error: " << errorString;
        });
    }
private:
    QSharedPointer<UsbDevice> device;
    QTimer readUsbTimer;
};

int main(int argc, char *argv[]) {
    QCoreApplication application(argc, argv);

    UsbDevManager::instance();
    UsbUser user;

    QObject::connect(&UsbDevManager::instance(), &UsbDevManager::deviceAttached, &user, &UsbUser::onDeviceAttached);
    QObject::connect(&UsbDevManager::instance(), &UsbDevManager::deviceDetached, &user, &UsbUser::onDeviceDetached);

    QTimer::singleShot(100, [&]() {
        UsbDevManager::instance().addMonitorId({0x4831, 0x4831});
    });

    application.exec();
}

#include "main.moc"

```
