#pragma once
#include "bean/commontype.h"
#include "iomanager.h"
#include "libusb.h"
#include "protocolcodecengine.h"
#include "utils/customplot/qcustomplot.h"
#include <qobject.h>
#include <qthread.h>
#include <qtimer.h>

PROTOCOL_CODEC_USING_NAMESPACE
//
#define PID 0x010c
#define VID 0x0d7d
////
//#define PID 0x0105
//#define VID 0x1d6b

#define CTL_POINT 1
#define DATA_POINT 1
#define SYNC_POINT 3
#define USB_DEVICE (UsbManager::instance().getDevice(PID, VID))

class UsbDevice : public QObject
{
    Q_OBJECT
public:
    enum OPERATION
    {
        OPEN_DEVICE = 1,
        DETACH_KERNEL = 1 << 1,
        SET_CONFIGURATION = 1 << 2,
        CLAIM_INTERFACE = 1 << 3,
        REGISTER_CALLBACK = 1 << 4,
    };
    Q_ENUM(OPERATION)

public:
    explicit UsbDevice(libusb_device *device, libusb_context *ctx = nullptr, uint16_t productId = LIBUSB_HOTPLUG_MATCH_ANY,
                       uint16_t vendorId = LIBUSB_HOTPLUG_MATCH_ANY, uint16_t classId = LIBUSB_HOTPLUG_MATCH_ANY, QObject *parent = nullptr);

    ~UsbDevice() override;

    void openDevice();

    bool isOpened() const;

//    void doSample(bool start);

    void insertEndPoint(uint8_t pointNumer, libusb_transfer_type transferType, IOType type);

    libusb_device_handle* getDeviceHandle();

    bool interfaceRequired(int configIndex, int interfaceIndex) const;

    void read(uint8_t point);

    void readCfgParams();

    template<typename T>
    void write(uint8_t point, T data) {
        auto contentSz = sizeof(data);
        auto codeData = codecEngine.encode(data);
        QByteArray rawData((int)contentSz, 0);
        memcpy(rawData.data(), &data, contentSz);
        insertIntoCache(data.Type, codeData);
    }
    void writeCtlData(DataType dataType);

public slots:
    void onTransferFinished(IOType ioType, QByteArray data, int cmdType);

    void onErrorOccurred(const QString& errStr);

    void onTimeOut(IOType ioType);
signals:
    // 同步控制传输
    void doSyncCtlIO(uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, IOType ioType, const QByteArray& data, int cmdType);
    // 同步批量传输
    void doSyncBulkIO(unsigned char endpoint, int length, IOType ioType, const QByteArray& data, int cmdType);
    // 同步中断传输
    void doSyncInterIo(unsigned char endpoint, int length, IOType ioType, const QByteArray& data, int cmdType);
    // 异步控制传输
    void doAsyncCtlIO(uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, IOType ioType, const QByteArray &data = QByteArray());
    // 异步中断传输
    void doAsyncInterIO(uint8_t endPoint, IOType ioType, const QByteArray& data = QByteArray());
    // 异步等时传输
    void doAsyncIsoIO(uint8_t endPoint, IOType ioType, uint8_t  pointNum, const QByteArray& data = QByteArray());

    void dataReady(const FrameData& data);

    void information(const  QString& errorStr, bool isError);

    void paramsReadFinished();
private:
    /**
     * @brief 注册热插拔回调函数，Windows不可用
     * @param arrive
     * @param lefty
     */
    void registerHotPlugCallback(libusb_hotplug_callback_fn arrive, libusb_hotplug_callback_fn left = nullptr);

    /**
     * @brief 取消注册的热插拔回调函数
     */
    void deregisterHotPlugCallback();

private:
    void write(uint8_t point, const QByteArray& data, int cmdType);

    /**
     * @brief 检查执行结果并打印错误
     * @param res
     * @param op
     * @return
     */
    bool checkOperationRes(int res, OPERATION op);

    void registerCodeC();

    void onFeedBackReceived(const Feedback& data);

    void onSampleDataReceived(const FrameData& data);

    void onAllParamsReceived(const AllParams& data);

    void checkCommandCache(Feedback feedback);

    void insertIntoCache(int cmd, const QByteArray& data);

    void initTimer();

    void writeNextCommand();

private:
    uint16_t productId, vendorId, classId;
    int usedConfigNum = 0, usedInterfaceNum = 0;
    QMap<uint8_t, IoManager::ThrType> pointTransType;
    int devValid = 0;
    libusb_device_handle *deviceHandle{nullptr};
    libusb_device *device;
    libusb_context *ctx;
    QString identifier{};
    libusb_hotplug_callback_handle hotPlugCallbackHandle[2];

    ProtocolCodecEngine codecEngine;
    QTimer sampleTimer;
    bool isWriting = false, isParamSync = false;
    QQueue<QPair<int, QByteArray>> writeCmdCache;
    bool stateError = true;
    qint64 bytesCount = 0, frameCount = 0 , wrongFrameCount = 0;
    QTimer bytesCountTm;
};

// Unix平台热插拔回调函数
static int LIBUSB_CALL onDeviceArrive(libusb_context *ctx, libusb_device *device, libusb_hotplug_event event, void *user_data) {
    return 0;
}
static int LIBUSB_CALL onDeviceLeft(libusb_context *ctx, libusb_device *device, libusb_hotplug_event event, void *user_data) {
    return 0;
}