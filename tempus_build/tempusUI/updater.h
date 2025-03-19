#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QDebug>
#include <QTimer>

class updater : public QObject
{
    Q_OBJECT

public:
    explicit updater(QObject *parent = nullptr);

public slots:
    void work();

signals:
    void updateUI(const QString &newText);

};


#endif
