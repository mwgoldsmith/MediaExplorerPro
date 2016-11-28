/********************************************************************************
** Form generated from reading UI file 'downloadspanel.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DOWNLOADSPANEL_H
#define UI_DOWNLOADSPANEL_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DownloadsPanel
{
public:
    QHBoxLayout *horizontalLayout;
    QLabel *label;

    void setupUi(QWidget *DownloadsPanel)
    {
        if (DownloadsPanel->objectName().isEmpty())
            DownloadsPanel->setObjectName(QStringLiteral("DownloadsPanel"));
        DownloadsPanel->resize(400, 300);
        horizontalLayout = new QHBoxLayout(DownloadsPanel);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, -1);
        label = new QLabel(DownloadsPanel);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);


        retranslateUi(DownloadsPanel);

        QMetaObject::connectSlotsByName(DownloadsPanel);
    } // setupUi

    void retranslateUi(QWidget *DownloadsPanel)
    {
        DownloadsPanel->setWindowTitle(QApplication::translate("DownloadsPanel", "Form", 0));
        label->setText(QApplication::translate("DownloadsPanel", "downloads", 0));
    } // retranslateUi

};

namespace Ui {
    class DownloadsPanel: public Ui_DownloadsPanel {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DOWNLOADSPANEL_H
