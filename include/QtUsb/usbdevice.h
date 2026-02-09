#pragma once

#include "QtUsb/usb_namespace.h"
#include "QtUsb/datatypes.h"
#include "QtUsb/qtusb_export.h"
#include <qobject.h>
#include <qflags.h>

QT_USB_NAMESPACE_BEGIN
class IoCommand;

class UsbDescriptor;

class QTUSB_API UsbDevice : public QObject {
    Q_OBJECT

public:
    explicit UsbDevice(UsbId usbId, QObject *parent = nullptr);

    ~UsbDevice();

    /**
     * @brief 设置当前设备句柄是否有效
     * @param valid
     * @note 当设备拔除时，当前设备句柄被设置为无效，IO操作会被忽略
     */
    void setValid(bool valid);

    /**
     * @brief 设置设备使用的配置
     * @note 如果需要开启速度打印，需要先调用此函数，之后再调用setSpeedPrintEnable()
     * @param newCfg
     */
    void setConfiguration(ActiveUSBConfig newCfg);

    /**
     * @brief 设备配置之后进行端点读取操作
     */
    void read() const;

    /**
     * @brief 设备配置之后进行数据写入操作
     * @param data, 右值传递待写入数据，在数据量较大时，减少内存拷贝
     */
    void write(QByteArray &&data) const;

    /**
     * @brief 设备配置之后进行数据写入操作
     * @param data，左值传递写入数据, 数据量少时，且方便外部调用
     */
    void write(const QByteArray &data) const;

    /**
     * @brief 开启/关闭传输速度打印
     * @note 需要在setConfiguration()之后调用才生效
     * @param enable
     */
    void setSpeedPrintEnable(bool readSpeed, bool writeSpeed);

    /**
     * @brief 打印USB配置信息
     */
    void printInfo() const;

    /**
     * @brief 检查设备是否有效(设备通过setConfiguration选择了有效的配置，并且没有被移除)
     * @return true: 有效
     */
    bool isDevValid() const;

    /**
     * @brief 返回设备ID
     */
    UsbId getUsbId() const;

    /**
     * @brief 返回当前配置
     */
    ActiveUSBConfig getCurCfg() const;
private:
    /**
     * @brief 打开设备
     * @param usbId
     * @param device
     */
    void openDevice();

    /**
     * @brief 检查IO操作是否可以进行，当设备断开或打开失败时，IO操作检查将失败
     * @param isRead 是否是读取操作
     * @return true: 设备IO可用 false: IO不可用
     */
    bool checkIOEnabled(bool isRead) const;

signals:
    void readFinished(const QByteArray &data);

    void writeFinished();

    void errorOccurred(int errorCode, const QString &errorString);

private:
    std::atomic<bool> validFlag{false};
    UsbId id;
    ActiveUSBConfig usbCfg;
    IoCommand *ioCommand{nullptr};
    libusb_device_handle *handle{nullptr};
};

QT_USB_NAMESPACE_END
