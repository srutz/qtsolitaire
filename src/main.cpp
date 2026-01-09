#include "mainwindow.h"
#include <QApplication>
#include <QByteArray>
#include <QDebug>
#include <QElapsedTimer>
#include <QFuture>
#include <QList>
#include <QPalette>
#include <QScreen>
#include <QStyleFactory>
#include <QThread>
#include <QVector>
#include <QtConcurrent>

int square(const int &x) { return x * x; }

auto square_ = [](const int &value) {
    QThread::msleep(100);
    return value * value;
};

int main(int argc, char *argv[])
{

    QVector<int> list;
    for (int i = 0; i < 100; ++i) {
        list.append(i);
    }
    QElapsedTimer t;
    t.start();
    QThreadPool::globalInstance()->setMaxThreadCount(10);
    auto future = QtConcurrent::mapped(list, square);
    // auto future = QtConcurrent::mapped(list, square);
    future.waitForFinished();

    auto dt = t.elapsed();
    qDebug() << "took" << dt << "ms";
    qDebug() << " list:" << list;
    return 0;

    qDebug() << "Qt version:" << qVersion();
    // QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    // QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication::setStyle(QStyleFactory::create("Fusion"));
    QApplication a(argc, argv);
    MainWindow w;
    w.resize(1100, 800);
    w.setWindowTitle("Qt-Solitaire");
    auto *screen = QGuiApplication::primaryScreen();
    auto screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - w.width()) / 2;
    int y = (screenGeometry.height() - w.height()) / 2;
    w.move(x, y);
    w.show();
    return QApplication::exec();
}
