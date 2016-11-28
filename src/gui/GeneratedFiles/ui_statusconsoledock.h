/********************************************************************************
** Form generated from reading UI file 'statusconsoledock.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_STATUSCONSOLEDOCK_H
#define UI_STATUSCONSOLEDOCK_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_StatusConsoleDock
{
public:

    void setupUi(QWidget *StatusConsoleDock)
    {
        if (StatusConsoleDock->objectName().isEmpty())
            StatusConsoleDock->setObjectName(QStringLiteral("StatusConsoleDock"));
        StatusConsoleDock->resize(699, 194);

        retranslateUi(StatusConsoleDock);

        QMetaObject::connectSlotsByName(StatusConsoleDock);
    } // setupUi

    void retranslateUi(QWidget *StatusConsoleDock)
    {
        StatusConsoleDock->setWindowTitle(QApplication::translate("StatusConsoleDock", "Form", 0));
    } // retranslateUi

};

namespace Ui {
    class StatusConsoleDock: public Ui_StatusConsoleDock {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_STATUSCONSOLEDOCK_H
