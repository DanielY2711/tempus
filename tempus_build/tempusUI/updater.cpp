#include "updater.h"
#include <QThread>
#include <QTimer>
#include "windowTracker.h"

updater::updater(QObject *parent) : QObject(parent)
{
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &updater::work);
    timer->start(500);
}

void updater::work()
{
    windowTracker wt;
    qDebug() << "Working" << QThread::currentThread();
    const QString newText = QString::fromStdString(wt.getWindowHeader());
    emit updateUI(newText);
}

