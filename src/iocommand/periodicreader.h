#pragma once

#include "iocommand.h"

#include <qthread.h>

QT_USB_NAMESPACE_BEGIN
class PeriodicReadWorker : public QObject {
    Q_OBJECT

public:
    explicit PeriodicReadWorker(IoCommand *ioCmd, QObject *parent = nullptr);

    ~PeriodicReadWorker();

signals:
    void startTimer(bool start, int interval = 0);

private slots:
    void onStartTimer(bool start, int interval = 0);

private:
    QTimer *timer{};
    IoCommand *ioCmd{};
};

class PeriodicReader : public QObject {
    Q_OBJECT

public:
    explicit PeriodicReader(IoCommand *ioCmd, QObject *parent = nullptr);
    ~PeriodicReader();

    void startPeriodicRead(bool start, int interval = 0);

private:
    PeriodicReadWorker *worker;
    QThread *thr{};
};

QT_USB_NAMESPACE_END
