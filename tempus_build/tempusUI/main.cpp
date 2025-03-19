#include "tempus.h"
#include <QApplication>
#include <QMainWindow>
#include <QLineEdit>
#include <QTimer>
#include <QVBoxLayout>


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    tempus w;
    w.show();

    return a.exec();
}
