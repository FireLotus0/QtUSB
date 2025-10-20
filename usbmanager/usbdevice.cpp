#include "usbdevice.h"
#include "controlparams.h"
#include "iomanager.h"
#include "usbinfo.h"
#include "utils/filemanager/filemanager.h"

#include <qdebug.h>

Q_LOGGING_CATEGORY(checkError, "checkError")

UsbDevice::UsbDevice(libusb_device *device, libusb_context *ctx, uint16_t productId, uint16_t vendorId, uint16_t classId, QObject *parent)
    : QObject(parent),
      device(device),
      ctx(ctx),
      productId(productId),
      vendorId(vendorId),
      classId(classId) {
    hotPlugCallbackHandle[0] = -1;
    hotPlugCallbackHandle[1] = -1;
    registerCodeC();
    initTimer();
}


bool UsbDevice::checkOperationRes(int res, OPERATION op) {
    if (res != 0) {
        qInfo() << "Operation Failed: PID: " << productId << " VID:" << vendorId << op << libusb_error_name(res);
        return false;
    }
    devValid |= op;
    return true;
}

UsbDevice::~UsbDevice() {
    sampleTimer.stop();
    IoManager::instance().removeDevice(this);
    if (isOpened()) {
        if (devValid & REGISTER_CALLBACK) {
            deregisterHotPlugCallback();
        }
        if (devValid & CLAIM_INTERFACE) {
            libusb_release_interface(deviceHandle, usedInterfaceNum);
        }
        libusb_close(deviceHandle);
    }
}

void UsbDevice::openDevice() {
    if (isOpened()) {
        qWarning() << "Open Device Failed: Device has been open! ";
        return;
    }
    int res = LIBUSB_ERROR_NO_DEVICE;
    if (productId != LIBUSB_HOTPLUG_MATCH_ANY && vendorId != LIBUSB_HOTPLUG_MATCH_ANY) {
        deviceHandle = libusb_open_device_with_vid_pid(ctx, vendorId, productId);
        if (deviceHandle != nullptr) {
            res = 0;
            devValid |= OPEN_DEVICE;
        } else {
            qWarning() << "Open Device Failed: " << libusb_error_name(res);
        }
    } else {
        Q_ASSERT(device != nullptr);
        res = libusb_open(device, &deviceHandle);
        checkOperationRes(res, OPEN_DEVICE);
        qWarning() << "Open Device Failed: " << libusb_error_name(res);
    }
    if (deviceHandle) {
        UsbInfo::resolveUsbInfo(this, productId, vendorId, classId);
#ifdef USE_ASYNC_IO
        IoManager::instance().startEvtHandle();
#endif
        // 注册回调函数
        registerHotPlugCallback(onDeviceArrive, onDeviceLeft);
        checkOperationRes(libusb_set_auto_detach_kernel_driver(deviceHandle, 1), DETACH_KERNEL);
        checkOperationRes(libusb_set_configuration(deviceHandle, usedConfigNum), SET_CONFIGURATION);
        checkOperationRes(libusb_claim_interface(deviceHandle, usedInterfaceNum), CLAIM_INTERFACE);
        stateError = false;
        sampleTimer.start();
        // 设备连接成功后，读取所有配置参数
        QTimer::singleShot(200, [&] {
            // 读取所有参数
            readCfgParams();
            //            write(CTL_POINT, QByteArray(1, 0x9), 2);
        });
    }
}

void UsbDevice::registerHotPlugCallback(libusb_hotplug_callback_fn arrive, libusb_hotplug_callback_fn left) {
    if (!isOpened()) {
        return;
    }
    int res;
    if (left == nullptr) {
        res = libusb_hotplug_register_callback(ctx, LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED | LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT, LIBUSB_HOTPLUG_ENUMERATE,
                                               vendorId, productId, classId, arrive, nullptr, &hotPlugCallbackHandle[0]);
        checkOperationRes(res, REGISTER_CALLBACK);
    } else {
        res = libusb_hotplug_register_callback(ctx, LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED, LIBUSB_HOTPLUG_ENUMERATE, vendorId, productId, classId,
                                               arrive, nullptr, &hotPlugCallbackHandle[0]);
        checkOperationRes(res, REGISTER_CALLBACK);
        res = libusb_hotplug_register_callback(ctx, LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT, LIBUSB_HOTPLUG_ENUMERATE, vendorId, productId, classId, left,
                                               nullptr, &hotPlugCallbackHandle[1]);
        checkOperationRes(res, REGISTER_CALLBACK);
    }
}

void UsbDevice::deregisterHotPlugCallback() {
    if (hotPlugCallbackHandle[0] != -1) {
        libusb_hotplug_deregister_callback(ctx, hotPlugCallbackHandle[0]);
    }
    if (hotPlugCallbackHandle[1] != -1) {
        libusb_hotplug_deregister_callback(ctx, hotPlugCallbackHandle[1]);
    }
    hotPlugCallbackHandle[0] = -1;
    hotPlugCallbackHandle[1] = -1;
}

bool UsbDevice::isOpened() const {
    return devValid != 0;
}
libusb_device_handle *UsbDevice::getDeviceHandle() {
    return deviceHandle;
}
void UsbDevice::insertEndPoint(uint8_t pointNumer, libusb_transfer_type transferType, IOType ioType) {
#ifdef USE_ASYNC_IO
    bool useAsync = true;
#else
    bool useAsync = false;
#endif
    auto &ioManager = IoManager::instance();
    IoManager::ThrType type;
    switch (transferType) {
        case libusb_transfer_type::LIBUSB_TRANSFER_TYPE_CONTROL:
            type = useAsync ? IoManager::ASYNC_CTL_IO : IoManager::SYNC_CTL_IO;
            ioManager.insertTransfer(pointNumer, type, ioType, this);
            break;
        case libusb_transfer_type::LIBUSB_TRANSFER_TYPE_BULK:
            ioManager.insertTransfer(pointNumer, IoManager::SYNC_BULK_IO, ioType, this);// 异步批量传输未定义
            type = IoManager::SYNC_BULK_IO;
            break;
        case libusb_transfer_type::LIBUSB_TRANSFER_TYPE_INTERRUPT:
            type = useAsync ? IoManager::ASYNC_INTER_IO : IoManager::SYNC_INTER_IO;
            ioManager.insertTransfer(pointNumer, type, ioType, this);
            break;
        case libusb_transfer_type::LIBUSB_TRANSFER_TYPE_ISOCHRONOUS:
            type = IoManager::ASYNC_ISO_IO;
            ioManager.insertTransfer(pointNumer, IoManager::ASYNC_ISO_IO, ioType, this);// 实时传输只有异步方式
            break;
        default:
            qWarning() << "Transfer Type Not Support!";
            Q_ASSERT(false);
    }
    pointTransType[pointNumer] = type;
}
bool UsbDevice::interfaceRequired(int configIndex, int interfaceIndex) const {
    return usedConfigNum == configIndex && usedInterfaceNum == interfaceIndex;
}
void UsbDevice::onErrorOccurred(const QString &errStr) {
    //    if(deviceHandle) {
    //        emit information(errStr, true);
    //    }
}

void UsbDevice::onTransferFinished(IOType ioType, QByteArray data, int cmdType) {
    stateError = false;
    if (ioType == IOType::WRITE) {
        //        communicateTimer.start();
    } else {
        codecEngine.appendBuffer(data);
        bytesCount += data.size();
        frameCount++;
    }
}

void UsbDevice::read(uint8_t point) {
    Q_ASSERT(pointTransType.contains(point));
    switch (pointTransType[point]) {
        case IoManager::SYNC_CTL_IO: {
            uint8_t bmRequestType = 0;
            uint16_t wLength = CTL_BUF_SIZE;// 数据阶段传输阶段
            bmRequestType |= (1 << 7);      // device to host
            bmRequestType |= (2 << 4);      // vendor
            bmRequestType |= 2;             // specific point
            uint8_t bRequest = 0;
            uint16_t wValue = 0;
            uint16_t wIndex = 0;// 端点0
            doSyncCtlIO(bmRequestType, bRequest, wValue, wIndex, wLength, IOType::READ, QByteArray(), 22);
        }
            Q_ASSERT(false);
            break;
        case IoManager::SYNC_INTER_IO: {
            //            doSyncInterIo(LIBUSB_ENDPOINT_IN | point, BUFFER_SIZE, IOType::READ, QByteArray(), -1);
            Q_ASSERT(false);
        } break;
        case IoManager::SYNC_BULK_IO: {
            doSyncBulkIO(LIBUSB_ENDPOINT_IN | point, BULK_BUF_SIZE, IOType::READ, QByteArray(), -1);
        } break;
        case IoManager::ASYNC_CTL_IO: {
            //            doAsyncCtlIO();
            Q_ASSERT(false);
        } break;
        case IoManager::ASYNC_INTER_IO: {
            //            doAsyncInterIO(LIBUSB_ENDPOINT_IN | point, IOType::READ, QByteArray());
            Q_ASSERT(false);
        } break;
        case IoManager::ASYNC_ISO_IO: {
            //            doAsyncIsoIO();
            Q_ASSERT(false);
        } break;
    }
}

void UsbDevice::write(uint8_t point, const QByteArray &data, int cmdType) {
    if (!pointTransType.contains(point)) {
        qWarning() << "device not connected! ";
        return;
    }
    switch (pointTransType[point]) {
        case IoManager::SYNC_CTL_IO: {
            //            doSyncCtlIO();
            Q_ASSERT(false);
        } break;
        case IoManager::SYNC_INTER_IO: {
            //            doSyncInterIo(LIBUSB_ENDPOINT_OUT | point, BUFFER_SIZE, IOType::WRITE, data, cmdType);
            Q_ASSERT(false);
        } break;
        case IoManager::SYNC_BULK_IO: {
            doSyncBulkIO(LIBUSB_ENDPOINT_OUT | point, data.size(), IOType::WRITE, data, cmdType);
        } break;
        case IoManager::ASYNC_CTL_IO: {
            //            doAsyncCtlIO();
            Q_ASSERT(false);
        } break;
        case IoManager::ASYNC_INTER_IO: {
            //            doAsyncInterIO(LIBUSB_ENDPOINT_OUT | point, IOType::WRITE, data);
            Q_ASSERT(false);
        } break;
        case IoManager::ASYNC_ISO_IO: {
            //            doAsyncIsoIO();
        } break;
    }
}

void UsbDevice::registerCodeC() {
    codecEngine.frameDeclare("H(FEFE)S2CV(CRC16)E(FEFF)");
    codecEngine.setVerifyFlags("SC");
    codecEngine.registerType<Mode, BytesCodec>();
    codecEngine.registerType<PZT, BytesCodec>();
    codecEngine.registerType<Temp, BytesCodec>();
    codecEngine.registerType<Filter, BytesCodec>();
    codecEngine.registerType<Gain, BytesCodec>();
    codecEngine.registerType<Offset, BytesCodec>();
    codecEngine.registerType<SrcFreq, BytesCodec>();
    codecEngine.registerType<FmFreq, BytesCodec>();
    codecEngine.registerType<FmFreqOffset, BytesCodec>();
    codecEngine.registerType<SrcOutAmp, BytesCodec>();
    codecEngine.registerType<SrcOutOffset, BytesCodec>();
    codecEngine.registerType<SrcInputPhase, BytesCodec>();
    codecEngine.registerType<TriFreq, BytesCodec>();
    codecEngine.registerType<TriAmp, BytesCodec>();
    codecEngine.registerType<TuneOffset, BytesCodec>();
    codecEngine.registerType<TuneOffsetPzt, BytesCodec>();
    codecEngine.registerType<TuneOffsetTemp, BytesCodec>();
    codecEngine.registerType<Switch, BytesCodec>();
    codecEngine.registerType<Impedance, BytesCodec>();
    codecEngine.registerType<SampleRate, BytesCodec>();
    codecEngine.registerType<PztLimVolH, BytesCodec>();
    codecEngine.registerType<PztLimVolL, BytesCodec>();
    codecEngine.registerType<TempLimVolH, BytesCodec>();
    codecEngine.registerType<TempLimVolL, BytesCodec>();
    codecEngine.registerType<ReadSampleCmd, BytesCodec>();
    codecEngine.registerType<ReadParamsCmd, BytesCodec>();

    //    codecEngine.setLogging(checkError);

    // 注册编码，没有下位机，暂不解码
    codecEngine.registerType<BytesCodec<Feedback>>(this, &UsbDevice::onFeedBackReceived);
    codecEngine.registerType<BytesCodec<FrameData>>(this, &UsbDevice::onSampleDataReceived);
    codecEngine.registerType<BytesCodec<AllParams>>(this, &UsbDevice::onAllParamsReceived);
}

void UsbDevice::onFeedBackReceived(const Feedback &data) {
    checkCommandCache(data);
    qDebug() << "收到配置数据反馈：" << data.toBytes();
}

void UsbDevice::writeCtlData(DataType dataType) {
    switch (dataType) {
        case DataType::MODE: {
            Mode mode;
            mode.data = AppSetting::ControlParams::mode() ? 1 : 0;
            write(CTL_POINT, mode);
            break;
        }
        case DataType::IMPEDANCE: {
            Impedance impedance;
            impedance.data = AppSetting::ControlParams::inputImpedance();
            qDebug() << "try to write " << impedance.data;
            write(CTL_POINT, impedance);
            break;
        }// 输入阻抗
        case DataType::RF_FREQ: {
            SrcFreq srcFreq;
            srcFreq.data = AppSetting::ControlParams::rfFreq();
            write(CTL_POINT, srcFreq);
            break;
        };// 信号源射频频率
        case DataType::FM_FREQ: {
            FmFreq fmFreq;
            fmFreq.data = AppSetting::ControlParams::fmFreq();
            write(CTL_POINT, fmFreq);
            break;
        };// 信号源FM调制频率
        case DataType::FM_OFFSET: {
            FmFreqOffset fmFreqOffset;
            fmFreqOffset.data = AppSetting::ControlParams::fmOffset();
            write(CTL_POINT, fmFreqOffset);
            break;
        };// 信号源FM频率偏移
        case DataType::SRC_OUT_AMP: {
            SrcOutAmp srcOutAmp;
            srcOutAmp.data = AppSetting::ControlParams::srcOutAmp();
            write(CTL_POINT, srcOutAmp);
            break;
        };// 信号源输出幅度
        case DataType::SRC_OUT_OFFSET: {
            SrcOutOffset srcOutOffset;
            srcOutOffset.data = AppSetting::ControlParams::srcOutOffset();
            write(CTL_POINT, srcOutOffset);
            break;
        };// 信号源输出偏移
        case DataType::SRC_INPUT_PHASE: {
            SrcInputPhase srcInputPhase;
            srcInputPhase.data = AppSetting::ControlParams::srcInputPhase();
            write(CTL_POINT, srcInputPhase);
            break;
        };// 输入信号相位
        case DataType::TRI_FREQ: {
            TriFreq triFreq;
            triFreq.data = AppSetting::ControlParams::triFreq();
            write(CTL_POINT, triFreq);
            break;
        };// 三角波频率设置
        case DataType::TRI_AMP: {
            TriAmp triAmp;
            triAmp.data = AppSetting::ControlParams::triAmp();
            write(CTL_POINT, triAmp);
            break;
        };// 三角波幅度设置
        case DataType::TUNE_OFFSET: {
            TuneOffset tuneOffset;
            tuneOffset.data = AppSetting::ControlParams::tuneOffset();
            write(CTL_POINT, tuneOffset);
            break;
        };// 调谐偏移参数设置
        case DataType::PZT_TUNE_OFFSET: {
            TuneOffsetPzt tuneOffsetPzt;
            tuneOffsetPzt.data = AppSetting::ControlParams::pztTuneOffset();
            write(CTL_POINT, tuneOffsetPzt);
            break;
        };// PZT调谐输出偏移
        case DataType::TEMP_TUNE_OFFSET: {
            TuneOffsetTemp tempTuneOffset;
            tempTuneOffset.data = AppSetting::ControlParams::tempTuneOffset();
            write(CTL_POINT, tempTuneOffset);
            break;
        };// Temp调谐输出偏移
        case DataType::FILTER: {
            Filter filter;
            filter.data = AppSetting::ControlParams::filter();
            write(CTL_POINT, filter);
            break;
        };// 低通滤波器
        case DataType::GAIN_CFG: {
            Gain gain;
            gain.data = AppSetting::ControlParams::gainCfg();
            write(CTL_POINT, gain);
            break;
        };// 增益
        case DataType::OFFSET: {
            Offset offset;
            offset.data = AppSetting::ControlParams::offset();
            write(CTL_POINT, offset);
            break;
        };// 偏移设置
        case DataType::PZT_PID: {
            PZT pzt;
            pzt.p = AppSetting::ControlParams::pztP();
            pzt.i = AppSetting::ControlParams::pztI();
            pzt.d = AppSetting::ControlParams::pztD();
            pzt.speed = AppSetting::ControlParams::pztSpeed();
            write(CTL_POINT, pzt);
            break;
        };
        case DataType::TEMP_PID: {
            Temp temp;
            temp.p = AppSetting::ControlParams::tempP();
            temp.i = AppSetting::ControlParams::tempI();
            temp.d = AppSetting::ControlParams::tempD();
            temp.speed = AppSetting::ControlParams::tempSpeed();
            write(CTL_POINT, temp);
            break;
        };
        case DataType::SWITCH_ENABLE: {
            Switch switches;
            switches.tri_wave = AppSetting::ControlParams::triWave();
            switches.pzt_enable = AppSetting::ControlParams::pztEnable();
            switches.pid_pzt_enable = AppSetting::ControlParams::pztPidEnable();
            switches.temp_enable = AppSetting::ControlParams::tempEnable();
            switches.pid_temp_enable = AppSetting::ControlParams::pidTempEnable();
            write(CTL_POINT, switches);
            break;
        };
        case DataType::SAMPLE_RATE: {
            SampleRate rate;
            rate.rate = AppSetting::ControlParams::sampleRate();
            write(CTL_POINT, rate);
        } break;
        // PZT高限压
        case DataType::PZT_LIM_VOL_HIGH: {
            PztLimVolH pztLimVolH;
            pztLimVolH.data = AppSetting::ControlParams::pztLimVolH();
            write(CTL_POINT, pztLimVolH);
        } break;
        // PZT低限压
        case DataType::PZT_LIM_VOL_LOW: {
            PztLimVolL pztLimVolL;
            pztLimVolL.data = AppSetting::ControlParams::pztLimVolL();
            write(CTL_POINT, pztLimVolL);
        } break;
        // TEMP高限压
        case DataType::TEMP_LIM_VOL_HIGH: {
            TempLimVolH tempLimVolH;
            tempLimVolH.data = AppSetting::ControlParams::tempLimVolH();
            write(CTL_POINT, tempLimVolH);
        } break;
            // TEMP低限压
        case DataType::TEMP_LIM_VOL_LOW: {
            TempLimVolL tempLimVolL;
            tempLimVolL.data = AppSetting::ControlParams::tempLimVolL();
            write(CTL_POINT, tempLimVolL);
        } break;
        default: {
            Q_ASSERT(false);
        };
    }
}

void UsbDevice::onSampleDataReceived(const FrameData &data) {
    //    static uint32_t lastFrameId = INT_MAX;
    //    if(lastFrameId == INT_MAX) {
    //        lastFrameId = data.frameId;
    //    } else {
    //        if(lastFrameId + 1 != data.frameId) {
    //            wrongFrameCount++;
    ////            qWarning() << "帧ID不连续: last=" << lastFrameId << " current=" << data.frameId;
    //        }
    //        lastFrameId = data.frameId;
    //    }
    /// TODO:开启参数同步判断
    if (isParamSync) {
        emit dataReady(data);
    }
}

void UsbDevice::onAllParamsReceived(const AllParams &data) {
    isParamSync = true;
    qDebug() << "读取参数成功！";
    AppSetting::ControlParams::mode = data.mode == 1;
    AppSetting::ControlParams::inputImpedance() = data.inputImpedance & 1;
    AppSetting::ControlParams::rfFreq() = data.rfFreq;
    AppSetting::ControlParams::fmFreq() = data.fmFreq;
    AppSetting::ControlParams::fmOffset() = data.fmOffset;
    AppSetting::ControlParams::srcOutAmp() = data.srcOutAmp;
    AppSetting::ControlParams::srcOutOffset() = data.srcOutOffset;
    AppSetting::ControlParams::srcInputPhase() = data.srcInputPhase;
    AppSetting::ControlParams::triFreq() = data.triFreq;
    AppSetting::ControlParams::triAmp() = data.triAmp;
    AppSetting::ControlParams::tuneOffset() = data.tuneOffset;
    AppSetting::ControlParams::pztTuneOffset() = data.pztTuneOffset;
    AppSetting::ControlParams::tempTuneOffset() = data.tempTuneOffset;
    AppSetting::ControlParams::filter() = data.filter;
    AppSetting::ControlParams::gainCfg() = data.gainCfg;
    AppSetting::ControlParams::offset() = data.offset;
    AppSetting::ControlParams::pztP() = data.pztP;
    AppSetting::ControlParams::pztI() = data.pztI;
    AppSetting::ControlParams::pztD() = data.pztD;
    AppSetting::ControlParams::pztSpeed() = data.pztSpeed;
    AppSetting::ControlParams::tempP() = data.tempP;
    AppSetting::ControlParams::tempI() = data.tempI;
    AppSetting::ControlParams::tempD() = data.tempD;
    AppSetting::ControlParams::tempSpeed() = data.tempSpeed;
    AppSetting::ControlParams::triWave() = data.switches & 1;

    AppSetting::ControlParams::pztEnable() = data.switches & 2;
    AppSetting::ControlParams::pztPidEnable() = data.switches & 4;
    AppSetting::ControlParams::tempEnable() = data.switches & 8;
    AppSetting::ControlParams::pidTempEnable() = data.switches & 16;

    // 调制输出，温度控制，正交选择
    AppSetting::ControlParams::moduleOut() = data.inputImpedance & 2;
    AppSetting::ControlParams::tempAdjust() = data.inputImpedance & 4;
    AppSetting::ControlParams::ortho() = data.inputImpedance & 8;
    AppSetting::ControlParams::noModuleOut() = data.inputImpedance & 16;
    // pzt限压
    QByteArray tmp;
    tmp.resize(4);
    memcpy(tmp.data(), &data.tempLimVolL, 4);

    AppSetting::ControlParams::pztLimVolH() = data.pztLimVolH;
    AppSetting::ControlParams::pztLimVolL() = data.pztLimVolL;
    // Temp限压
    AppSetting::ControlParams::tempLimVolH() = data.tempLimVolH;
    AppSetting::ControlParams::tempLimVolL() = data.tempLimVolL;

    // 检查命令缓存
    isWriting = false;
    writeNextCommand();
    emit paramsReadFinished();
    information("参数同步成功!", false);
}

void UsbDevice::checkCommandCache(Feedback feedback) {
    auto cmd = feedback.command;
    if (writeCmdCache.isEmpty()) {
        qDebug() << "cache is empty , return;";
        return;
    } else if (feedback.command != writeCmdCache.front().first) {
        information("Command cache front not match with received!", true);
        qCritical() << QString("received command is %1  cache command is %2").arg(feedback.command).arg(writeCmdCache.first().first);
        Q_ASSERT(false);
    } else if (feedback.res == 2) {
    } else if (feedback.res == 1) {
        information(QString("Operation Failed!").arg(feedback.command), true);
        insertIntoCache(writeCmdCache.front().first, writeCmdCache.front().second);
    } else {
        //        information(QString("Success").arg(cmd), false);
        qDebug() << "cache is not empty , write next";
        writeNextCommand();
    }
}

void UsbDevice::onTimeOut(IOType ioType) {
    if (!stateError) {
        stateError = true;
        information("USB TimeOut!", true);
        if (ioType == IOType::WRITE && !writeCmdCache.isEmpty()) {
            write(CTL_POINT, writeCmdCache.front().second, writeCmdCache.front().first);
        } else {
            read(CTL_POINT);
        }
    }
}

void UsbDevice::insertIntoCache(int cmd, const QByteArray &data) {
    auto pair = qMakePair(cmd, data);
    if (writeCmdCache.isEmpty() || (!writeCmdCache.contains(pair))) {
        writeCmdCache.push_back(pair);
    } else {
        for (int i = writeCmdCache.size() - 1; i > 0; --i) {
            if (writeCmdCache[i] == pair) {
                writeCmdCache.removeAt(i);
            }
        }
        writeCmdCache.push_back(pair);
    }
    if (!isWriting) {
//        qDebug() << "isWriting is false start write!";
        write(CTL_POINT, writeCmdCache.front().second, writeCmdCache.front().first);
        isWriting = true;
    } else {
//        qDebug() << "isWriting is true, can not write!";
    }
}
void UsbDevice::initTimer() {
    sampleTimer.setInterval(AppSetting::ControlParams::sampleRate());
    sampleTimer.callOnTimeout([&] {
        read(DATA_POINT);
    });
    sampleTimer.setInterval(1);
    bytesCountTm.setInterval(1000);
    bytesCountTm.callOnTimeout([&] {
        //        qDebug() << "Speed: " << bytesCount / (1024 * 1024 * 1.0) << " MB/S" << " Frame Count=" << frameCount << " 不连续帧统计：" << wrongFrameCount;
        bytesCount = 0;
        wrongFrameCount = 0;
        frameCount = 0;
    });
    //    bytesCountTm.start();
}
void UsbDevice::writeNextCommand() {
    writeCmdCache.removeFirst();
    if (!writeCmdCache.isEmpty()) {
//        qDebug() << "writeCmdCache is not empty!";
        write(CTL_POINT, writeCmdCache.front().second, writeCmdCache.front().first);
    } else {
//        qDebug() << "cache is empty after remove";
        isWriting = false;
    }
}
void UsbDevice::readCfgParams() {
    qDebug() << "读取参数配置";
    ReadParamsCmd cmd;
    write(CTL_POINT, cmd);
}
