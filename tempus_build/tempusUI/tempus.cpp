#include "tempus.h"
#include "./ui_tempus.h"
#include "G:\tempus\tempus_build\src\logic\chrono.h"
#include "updater.h"
#include <QThread>


tempus::tempus(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::tempus)
{
    ui->setupUi(this);

    QThread *thread = new QThread;
    updater *textupdate = new updater;

    textupdate->moveToThread(thread);

    connect(thread, &QThread::started, textupdate, &updater::work);
    connect(thread, &QThread::finished, textupdate, &updater::deleteLater);

    connect(textupdate, &updater::updateUI, this, &tempus::on_UpdateUI);

    thread->start();
}

tempus::~tempus()
{
    delete ui;
}

void tempus::on_actionshowtime_triggered()
{

}


void tempus::on_windowButton_clicked()
{
    qDebug() << "Button clicked!";

    const QString newval = "QString::fromStdString(getTime())";

    ui->lineEdit->setText(newval);

}


void tempus::on_UpdateUI(const QString &newText)
{

    ui->lineEdit->setText(newText);
}
