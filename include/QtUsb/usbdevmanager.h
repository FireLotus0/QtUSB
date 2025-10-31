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
    void onDeviceAttached(UsbId id, LibUsbDevWrap dev);
    void onDeviceDetached(UsbId id);

private:
    explicit UsbDevManager(QObject *parent = 0);

    ~UsbDevManager();

private:
    UsbMonitor *monitor;
    QMap<UsbId, QSharedPointer<UsbDevice>> devices;
};

QT_USB_NAMESPACE_END
