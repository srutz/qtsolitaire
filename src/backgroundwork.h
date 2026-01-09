#ifndef BACKGROUNDWORK_H
#define BACKGROUNDWORK_H

#include <functional>
#include <QThread>
#include <QPointer>
#include <QObject>
#include <QDebug>
#include <QTimer>



template<typename T>
void runInBackground(QObject* mainThreadContext,
                            std::function<T()> workFunc,
                            std::function<void(T)> resultCallback)
{
    auto thread = new QThread();
    thread->setObjectName("Worker-Thread");
    auto context = new QObject();
    context->moveToThread(thread);
    QPointer<QObject> safeContext = mainThreadContext;
    QObject::connect(
        thread,
        &QThread::started,
        context,
        [context, workFunc, resultCallback, thread, safeContext]() {
            qDebug() << "work on " << QThread::currentThread();
            T result = workFunc();
            QTimer::singleShot(0, safeContext, [resultCallback, result, safeContext]() {
                if (safeContext) {  // Check if it exists
                    resultCallback(result);
                } else {
                    qDebug() << "Context was deleted, skipping result callback";
                }
         });
         thread->quit();
        });
    QObject::connect(thread, &QThread::finished, context, &QObject::deleteLater);
    QObject::connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    thread->start();
}

#endif // BACKGROUNDWORK_H
