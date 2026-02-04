#pragma once


#include "libusb.h"
#include "usb_namespace.h"
#include "datatypes.h"
#include "QtUsb/qtusb_export.h"
#include <qmap.h>
#include <qsharedpointer.h>
#include <qobject.h>

QT_USB_NAMESPACE_BEGIN

class UsbDevice;
class UsbMonitor;

enum class UsbLogLevel {
    DEBUG,
    INFO,
    WARNING,
    CRITICAL
};

class QTUSB_API UsbDevManager : public QObject {
    Q_OBJECT

public:
    static UsbDevManager &instance();

    /**
     * @brief 添加需要监听的设备ID
     * @param id 设备PID，VID
     */
    void addMonitorId(UsbId id);

    /**
     * @brief 取消对指定设备的监听
     * @param id
     */
    void removeMonitorId(UsbId id);

    /**
     * @brief 添加需要监听的设备类别
     * @param devClass 设备类别
     */
    void addMonitorClass(DeviceType deviceType);

    /**
     * @brief 取消对指定设备类别的监听
     * @param devClass 设备类别
     */
    void removeMonitorClass(DeviceType deviceType);

    /**
     * @brief 通过ID获取设备指针
     * @param id
     * @return
     */
    QSharedPointer<UsbDevice> getDevice(UsbId id) const;

    /**
     * @brief 设置日志输出级别
     * @param level
     */
    void setLogLevel(UsbLogLevel level);

    /**
     * @brief 释放libusb
     * @note 
     */
    void releaseUsbCxt();
signals:
    /**
     * @brief 设备插入
     * @param id
     */
    void deviceAttached(UsbId id);

    /**
     * @brief 设备拔除
     * @param id
     */
    void deviceDetached(UsbId id);

private slots:
    void onDeviceAttached(UsbId id);
    void onDeviceDetached(UsbId id);

private:
    explicit UsbDevManager(QObject *parent = 0);

    ~UsbDevManager();

private:
    UsbMonitor *monitor;
    QMap<UsbId, QSharedPointer<UsbDevice>> devices;
    // 当设备断开，将devices[id]放入到这里，保证最终的释放是UsbDevManager，如果将QSharedPointer<UsbDevice>放入到线程中，在线程退出时，如果QSharedPointer引用计数为0，
    // 就会导致在线程中释放对应的UsbDevice，从而出现跨线程操。
    QList<QSharedPointer<UsbDevice>> toDeleteDevs;
};

QT_USB_NAMESPACE_END
