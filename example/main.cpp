#include <qdebug.h>
#include <qcoreapplication.h>
#include <qthread.h>
#include <qtimer.h>

class Test : public QObject {
    Q_OBJECT
public:
    static Test& instance() {
        static Test instance;
        return instance;
    }
private:
    Test(QObject *parent = nullptr) : QObject(parent) {}

signals:
    void triggered();
};

int main(int argc, char *argv[]) {
    QCoreApplication application(argc, argv);

    QObject::connect(&Test::instance(), &Test::triggered, [&]() {
        qDebug() << "triggered";
    });
    auto thread = QThread::create([] {
           Test::instance().triggered();
       });
    QTimer::singleShot(100, [&]() {
        thread->start();
    });

    application.exec();
}

#include "main.moc"