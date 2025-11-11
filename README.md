# QtUsb

基于 Qt 封装的 libusb，简化常规USB数据传输，使用者***不需要关心设备创建，接口声明，设备释放等***操作，提供以下功能：
1. 根据设备PID，VID或设备类别(例如U盘)，监听设备热插拔事件
2. 通过接口read，write进行数据传输，数据传输是异步进行，传输完成触发信号通知使用者

---

## 目录

- [功能特点](#功能特点)
- [环境要求](#环境要求)
- [快速开始](#快速开始)
- [库结构说明](#库结构说明)
- [类与接口说明](#类说明)
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
如何集成库：

- 如何将封装库加入你的 Qt 项目
```
find_package(QtUsb REQUIRED)
//...
target_link_libraries (${PROJECT_NAME} PUBLIC QtUsb::QtUsb)
// 如果链接静态库，添加：target_compile_definitions(${PROJECT_NAME} PUBLIC QTUSB_STATIC)、
```
- 包含头文件
```c++
#include "QtUsb/usbdevmanager.h"
#include "QtUsb/usbdevice.h"

// 命令空间声明
USING_QT_USB_NAMESPACE
```
- 使用步骤
1. 监听热插拔信号: UsbDevManager::deviceAttached， UsbDevManager::deviceDetached
2. 添加要目标设备ID，可监听多个设备：UsbDevManager::instance().addMonitorId({0x4831, 0x4831});
3. 通过ID获取设备: UsbDevManager::instance().getDevice(id);
4. 选择配置： device->setConfiguration(ActiveUSBConfig{1, 0, 1, 8});
5. 监听设备信号: UsbDevice::readFinished，  UsbDevice::writeFinished，  UsbDevice::errorOccurred
6. 读写：read， write
---

## 库结构说明
```
QtUsb/
├── CMakeLists.txt                # 顶层 CMake 构建脚本
├── include/                      # 公共接口头文件目录
│   └── QtUsb/
├── src/                         # 源代码目录
│   ├── datatypes/               # 数据结构体定义
│   ├── descriptor/              # 设备描述符解析
│   ├── iocommand/               # IO命令处理，对IO命令进行同步，可以添加日志记录，数据打印等
│   ├── transfer/                # 进行实际的数据传输：批量传输，中断传输，控制传输
│   ├── usbdevice/               # 设备对象类
│   ├── usbdevmanager/           # 设备管理，单例
│   └── usbmonitor/              # 设备热插拔监听
├── examples/                    # 测试代码,用于本地调试
├── cmake/                       # CMake 模块和配置文件模板
└── README.md                    # 项目说明文档
```
---

## 类说明

### 1. struct ActiveUSBConfig
描述了设备使用的配置信息：配置，接口，端点。由于传输方式(控制，中断，批量)是由端点描述符决定的，所以当用户获取到UsbDevice时，***必须调用setConfiguration
进行设置，这样用户只管使用read,write即可，不用关心具体使用哪种传输方式***，库内部根据ActiveUSBConfig自动选择传输方式。对于***readCacheSize***，在
以往的开发中，***读取大量数据时，读取缓冲区大小对传输速度具有重要的影响***，因此提供该参数由用户自己进行设置。而***queuedCommands***，则是为了方便实现一些应用层面上的处理
逻辑，例如自定义通信协议时：请求1-->响应1, 请求2-->响应2......
```c++
struct QTUSB_API ActiveUSBConfig {
    quint8 configuration = 0xFF;    // 使用的配置ID
    quint8 interface = 0xFF;        // 接口
    uint8_t pointNumber = 0xFF;     // 端点
    int readCacheSize = 1024;       // 读取缓冲区大小
    bool queuedCommands{false};     // USB 2.0半双工传输，读写操作都是配对进行，USB 3.0支持全双工，设置为true，强制进行命令排队，实现命令同步
};
```


### 2. UsbDevManager
这是一个单例类。主要是代替用户进行USB设备管理。当设备到来时，UsbDevManager会主动打开设备，无论是否打开成功，
***都将创建QSharedPointer\<UsbDevice>实例，并进行管理，当设备拔除时，释放其智能指针。因此，即使用户获取到设备，如果打开失败，进行读写也会失败。
当设备打开成功时，会打印设备的描述符信息：设备描述符，配置描述符，接口描述符，端点描述符，在UsbDevice::setConfiguration时，可以根据这些信息进行设置***。

| 接口            | 说明                                                          |
|---------------|-------------------------------------------------------------|
| addMonitorId     | 添加需要监听的设备                                                   |
| removeMonitorId | 取消对指定设备的监听                                                  |
| addMonitorClass     | 根据设备类别添加监听                                                  |
| removeMonitorClass | 取消对指定类别的监听                                                  |
| getDevice | 获取设备， 返回QSharedPointer<UsbDevice>，需要检查是否为nullptr(当设备不存在时为空) |
| setLogLevel | 设置内部日志输出级别： DEBUG，INFO，WARNING，CRITICAL                     |

| 信号              | 说明                                                         |
|-----------------|------------------------------------------------------------|
| void deviceAttached(UsbId id)    | 设备插入  |
| void deviceDetached(UsbId id) | 设备拔除  |

### 3. UsbDevice
USB设备对象，提供接口进行数据传输。

| 接口               | 说明                                                                                                                 |
|------------------|--------------------------------------------------------------------------------------------------------------------|
| setValid         | 当设备拔除时，UsbDevManager会将设备设置为无效（考虑到用户可能会将QSharedPointer<UsbDevice>放到线程中使用，<br/>所以这是一个原子操作），IO操作会被忽略，***使用者通常不需要调用*** |
| setConfiguration | 选择需要的配置，参数为   ActiveUSBConfig结构体                                                                                   |
| read             | 读取数据，当设备拔出或没有选择配置时，读取操作无效                                                                                          |
| write            | 写入数据，当设备拔出或没有选择配置时，写入操作无效                                                                                          |
| printInfo        | 打印设备配置新信息                                              |


| 信号              | 说明     |
|-----------------|--------|
|  void readFinished(const QByteArray &data)    | 数据读取完成 |
| void writeFinished() | 数据写入完成 |
| void errorOccurred(int errorCode, const QString& errorString) | 数据传输错误 |

---
## 示例代码

```cpp
#include "QtUsb/usbdevmanager.h"
#include "QtUsb/usbdevice.h"
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
        device = UsbDevManager::instance().getDevice(id);
        if (device) {
            device->setSpeedPrintEnable(true);
            initUsbSig();
            device->setConfiguration({1, 0, 1, 8});
            readUsbTimer.start();
        }
    }

    void onDeviceDetached(UsbId id) {
        readUsbTimer.stop();
        device.reset();
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

    UsbDevManager::instance().setLogLevel(QT_USB::UsbLogLevel::WARNING);
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
