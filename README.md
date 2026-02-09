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

- Qt5, Qt6
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
5. 监听设备信号: UsbDevice::readFinished， UsbDevice::writeFinished， UsbDevice::errorOccurred
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

- 描述了设备使用的配置信息：获取到UsbDevice后，***必须调用setConfiguration()进行配置选择***
- 读取大量数据时，读取缓冲区大小对传输速度具有重要的影响，readCacheSize字段用于设置读取缓冲区的大小，每一次读取最多允许读取readCacheSize字节的数据，库内部会调整readCacheSize向上对齐到端点maxPacketSize整数倍
- queuedCommands字段，为了方便实现一些应用层面上的处理逻辑，例如自定义通信协议时：请求1-->响应1, 请求2-->响应2...,
- ***特别注意：需要兼顾数据采集和指令控制时，确保queuedCommands=false，这样写命令的优先级高于读命令，保证控制指令的及时性***

```c++
// 调用setConfiguration()时，参数需要一个ActiveUSBConfig对象，建议先调用printInfo()打印配置信息
struct QTUSB_API ActiveUSBConfig {
uint8_t configuration = 0xFF;   // 使用的配置ID
uint8_t interface = 0xFF;       // 接口
uint8_t pointNumber = 0xFF;     // 端点
int readCacheSize = 1024;       // 读取缓冲区大小
bool queuedCommands{ false };     // USB 2.0半双工传输，读写操作都是配对进行，USB 3.0支持全双工，设置为true，强制进行命令排队，实现命令同步
/*
 * 在使用单片机USB实现中断传输时，在Windows平台上，libusb返回实际传输（读/写）的
 * 字节数时会多出1字节, Linux平台上正常。抓包分析，发送的数据确实多出一个字节，值为0。
 * 例如：写入64字节数据，但是Windows平台上写入了65字节数据，即使设备最大包大小仅为64字节，此时设置discardBytes=1，
 *      每次分包写入时减去discardBytes，从而避免分包错误。当读取数据时，如果discardBytes > 0，
 *      则会将缓冲区大小设置为端点最大包大小sg + discardBytes，
 *      并且拷贝数据忽略后面的discardBytes个字节数据。目前只在单片机中断传输中使用，并且可能跟设备有关！
 * */
uint8_t discardBytes = 0;
/*
 * 在使用单片机USB传输时，连续发送命令，单片机可能无法处理。读取和写入的时间间隔虽然可以由外部调用来控制， 
 * 但是内部传输数据时如果存在分包，则存在多次命令写入，即使外部只调用一次read或write，因此添加这个字段进行控制。
 * 例如：单片机每次只能写入64字节，并且处理时间需要5ms，则设置cmdInterval = 5，当写入100字节的数据时，先写64字节，sleep(5),
 *      再写36字节...。目前只在中断传输和批量传输中使用，跟设备处理性能有关。
 * */
uint8_t cmdInterval = 0;
};
```

### 2. UsbDevManager

- 这是一个单例类，进行USB设备管理。
- UsbId用于标识设备，结构为struct { pid, vid}

| 接口                                  | 说明                                                                                         |
|-------------------------------------|--------------------------------------------------------------------------------------------|
| addMonitorId(UsbId id)              | 添加需要监听的设备，通过id指定                                                                           |
| removeMonitorId(UsbId id)           | 取消对指定设备的监听，通过id指定                                                                          |
| addMonitorClass(DeviceType type)    | 根据设备类别添加监听，通过枚举DeviceType指定，例如：DeviceType::USB_CLASS_MASS_STORAGE                          |
| removeMonitorClass(DeviceType type) | 取消对指定类别的监听                                                                                 |
| getDevice(UsbId id)                 | 获取设备，返回QSharedPointer<UsbDevice>，需要检查是否为nullptr(当设备不存在时为空)，获取到的QSharedPointer可以在其他线程里面安全使用 |
| setLogLevel(UsbLogLevel level)      | 设置内部日志输出级别： DEBUG，INFO，WARNING，CRITICAL                                                    |
| releaseUsbCxt()                     | 程序退出时会自动调用此函数释放libusb，***通常使用者只需要关注，在程序异常终止（例如Ctrl C按下），监听此类事件，然后调用此函数***                  |

| 信号                            | 说明           |
|-------------------------------|--------------|
| void deviceAttached(UsbId id) | 设备插入时emit此信号 |
| void deviceDetached(UsbId id) | 设备拔除时emit此信号 |

### 3. UsbDevice

USB设备对象，提供接口进行数据传输。

| 接口                  | 说明                                                                                                     |
|---------------------|--------------------------------------------------------------------------------------------------------|
| setValid            | 当设备拔除时，UsbDevManager会自动将设备设置为无效（原子操作），无效时，UsbDevice虽然可以调用read, write， 但是实际IO操作会被忽略，***使用者通常不需要调用此函数*** |
| setConfiguration    | 选择需要的配置，参数为ActiveUSBConfig结构体， ***建议先调用printInfo()查看配置信息***                                            |
| read                | 读取数据，当设备拔出或没有选择配置时，读取操作无效                                                                              |
| write               | 写入数据，当设备拔出或没有选择配置时，写入操作无效                                                                              |
| printInfo           | 打印设备配置信息                                                                                               |
| setSpeedPrintEnable | 打印每秒接收到的数据量，单位为KB/S或MB/S，***注意：此函数在setConfiguration()之后调用才能生效***                                       |   

| 信号                                                            | 说明     |
|---------------------------------------------------------------|--------|
| void readFinished(const QByteArray &data)                     | 数据读取完成 |
| void writeFinished()                                          | 数据写入完成 |
| void errorOccurred(int errorCode, const QString& errorString) | USB错误  |

---

## 示例代码

```cpp
#include "QtUsb/usbdevmanager.h"
#include "QtUsb/usbdevice.h"
#include <qdebug.h>
#include <qcoreapplication.h>
#include <qtimer.h>

USING_QT_USB_NAMESPACE

// 测试类
class UsbUser : public QObject {
    Q_OBJECT

public:
    explicit UsbUser(QObject *parent = 0) : QObject(parent) {
        // 定时读取Usb数据
        readUsbTimer.setInterval(100);
        readUsbTimer.callOnTimeout([&] {
            device->read();
        });
    }

public slots:
    // 设备插入时的回调函数
    void onDeviceAttached(UsbId id) {
        // 获取设备指针
        device = UsbDevManager::instance().getDevice(id);
        if (device) {
            // 打印配置信息，在setConfiguration之前建议先打印相关信息
            device->printInfo();
            // 开启读取速度打印
            device->setSpeedPrintEnable(true);
            // 连接IO操作相关的信号
            initUsbSig();
            // 设置配置
            device->setConfiguration({1, 0, 1, 8});
            // 开启定时读取，此处为示例，不一定需要在这里开启定时器读取数据
            readUsbTimer.start();
        }
    }

    // 设备断开回调
    void onDeviceDetached(UsbId id) {
        readUsbTimer.stop();
        // 可以不用调此函数
        device.reset();
    }

private:
    // 连接IO信号
    void initUsbSig() {
        
        // 数据读取完成处理
        connect(device.get(), &UsbDevice::readFinished, this, [&](const QByteArray &data) {
            qDebug() << "read data: " << data.toHex(' ');
        });
        // 数据写入完成处理
        connect(device.get(), &UsbDevice::writeFinished, this, [&]() {
            qDebug() << "write finished ";
        });
        // 错误处理
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

    // 设置日志输出级别
    UsbDevManager::instance().setLogLevel(QT_USB::UsbLogLevel::WARNING);
    UsbUser user;

    // 设备插拔处理，在添加监听之前连接信号
    QObject::connect(&UsbDevManager::instance(), &UsbDevManager::deviceAttached, &user, &UsbUser::onDeviceAttached);
    QObject::connect(&UsbDevManager::instance(), &UsbDevManager::deviceDetached, &user, &UsbUser::onDeviceDetached);

    // 添加需要监听插拔的设备ID
    UsbDevManager::instance().addMonitorId({0x4831, 0x4831});

    application.exec();
}

#include "main.moc"



```
