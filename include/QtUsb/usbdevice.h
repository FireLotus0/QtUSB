#pragma once

#include "QtUsb/usb_namespace.h"
#include "QtUsb/datatypes.h"
#include "QtUsb/qtusb_export.h"
#include <qobject.h>

QT_USB_NAMESPACE_BEGIN
class IoCommand;
class UsbDescriptor;

class QTUSB_API UsbDevice : public QObject {
    Q_OBJECT

public:
    explicit UsbDevice(UsbId usbId, libusb_device *device, QObject *parent = nullptr);

    ~UsbDevice();

    /**
     * @brief 设置当前设备句柄是否有效
     * @param valid
     * @note 当设备拔除时，当前设备句柄被设置为无效，IO操作会被忽略
     */
    void setValid(bool valid);

    /**
     * @brief 设置设备使用的配置
     * @param newCfg
     */
    void setConfiguration(ActiveUSBConfig newCfg);

    /**
     * @brief 设备配置之后进行端点读取操作
     */
    void read() const;

    /**
     * @brief 设备配置之后进行数据写入操作
     * @param data
     */
    void write(QByteArray &&data) const;

    /**
     * @brief 开启/关闭传输速度打印
     * @param enable
     */
    void setSpeedPrintEnable(bool enable);

private:
    /**
     * @brief 打开设备。当监听到设备插入时，在UsbDevManager自动进行打开操作，若成功，会设置validFlag=true，后续IO操作可顺利进行
     * @param usbId
     * @param device
     */
    void openDevice();

signals:
    void readFinished(const QByteArray &data);
    void writeFinished();
    void errorOccurred(int errorCode, const QString& errorString);

private:
    std::atomic<bool> validFlag{false};
    UsbId id;
    libusb_device* device{nullptr};
    UsbDescriptor* descriptor{nullptr};
    ActiveUSBConfig usbCfg;
    IoCommand *ioCommand{nullptr};
    libusb_device_handle *handle{nullptr};
};

QT_USB_NAMESPACE_END
