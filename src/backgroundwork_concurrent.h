#include <QFuture>
#include <QtConcurrent>

template <typename T>
void runInBackgroundConcurrent(QObject *mainThreadContext, std::function<T()> workFunc, std::function<void(T)> resultCallback)
{
    if (!mainThreadContext) {
        return;
    }
    QPointer<QObject> safeContext = mainThreadContext;
    QFuture<T> future = QtConcurrent::run(workFunc);
    auto *watcher = new QFutureWatcher<T>(mainThreadContext);
    QObject::connect(watcher, &QFutureWatcher<T>::finished, mainThreadContext, [watcher, resultCallback, safeContext]() {
        if (safeContext) {
            resultCallback(watcher->result());
        }
        watcher->deleteLater();
    });
    watcher->setFuture(future);
}
