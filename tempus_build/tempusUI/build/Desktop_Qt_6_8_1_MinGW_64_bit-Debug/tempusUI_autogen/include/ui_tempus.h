/********************************************************************************
** Form generated from reading UI file 'tempus.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TEMPUS_H
#define UI_TEMPUS_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_tempus
{
public:
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;
    QPushButton *windowButton;
    QLineEdit *lineEdit;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *tempus)
    {
        if (tempus->objectName().isEmpty())
            tempus->setObjectName("tempus");
        tempus->resize(800, 600);
        centralwidget = new QWidget(tempus);
        centralwidget->setObjectName("centralwidget");
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setObjectName("horizontalLayout");
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        windowButton = new QPushButton(centralwidget);
        windowButton->setObjectName("windowButton");

        verticalLayout->addWidget(windowButton);

        lineEdit = new QLineEdit(centralwidget);
        lineEdit->setObjectName("lineEdit");

        verticalLayout->addWidget(lineEdit);


        horizontalLayout->addLayout(verticalLayout);

        tempus->setCentralWidget(centralwidget);
        menubar = new QMenuBar(tempus);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 21));
        tempus->setMenuBar(menubar);
        statusbar = new QStatusBar(tempus);
        statusbar->setObjectName("statusbar");
        tempus->setStatusBar(statusbar);

        retranslateUi(tempus);

        QMetaObject::connectSlotsByName(tempus);
    } // setupUi

    void retranslateUi(QMainWindow *tempus)
    {
        tempus->setWindowTitle(QCoreApplication::translate("tempus", "tempus", nullptr));
        windowButton->setText(QCoreApplication::translate("tempus", "Check Window", nullptr));
        lineEdit->setText(QCoreApplication::translate("tempus", "This text updates every second", nullptr));
    } // retranslateUi

};

namespace Ui {
    class tempus: public Ui_tempus {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TEMPUS_H
