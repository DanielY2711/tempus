#ifndef TEMPUS_H
#define TEMPUS_H

#include <QMainWindow>
#include <QLineEdit>

QT_BEGIN_NAMESPACE
namespace Ui {
class tempus;
}
QT_END_NAMESPACE

class tempus : public QMainWindow
{
    Q_OBJECT

public:
    tempus(QWidget *parent = nullptr);
    ~tempus();

private slots:
    void on_actionshowtime_triggered();
    void on_windowButton_clicked();
    void on_UpdateUI(const QString &newText);

private:
    Ui::tempus *ui;
    QTimer *timer;
};
#endif // TEMPUS_H
