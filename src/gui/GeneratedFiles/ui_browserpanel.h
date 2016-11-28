/********************************************************************************
** Form generated from reading UI file 'browserpanel.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BROWSERPANEL_H
#define UI_BROWSERPANEL_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_BrowserPanel
{
public:
    QHBoxLayout *horizontalLayout;
    QLabel *label;

    void setupUi(QWidget *BrowserPanel)
    {
        if (BrowserPanel->objectName().isEmpty())
            BrowserPanel->setObjectName(QStringLiteral("BrowserPanel"));
        BrowserPanel->resize(400, 300);
        horizontalLayout = new QHBoxLayout(BrowserPanel);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(BrowserPanel);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);


        retranslateUi(BrowserPanel);

        QMetaObject::connectSlotsByName(BrowserPanel);
    } // setupUi

    void retranslateUi(QWidget *BrowserPanel)
    {
        BrowserPanel->setWindowTitle(QApplication::translate("BrowserPanel", "Form", 0));
        label->setText(QApplication::translate("BrowserPanel", "browser", 0));
    } // retranslateUi

};

namespace Ui {
    class BrowserPanel: public Ui_BrowserPanel {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BROWSERPANEL_H
