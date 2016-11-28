/********************************************************************************
** Form generated from reading UI file 'clipviewerdock.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CLIPVIEWERDOCK_H
#define UI_CLIPVIEWERDOCK_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ClipViewerDock
{
public:

    void setupUi(QWidget *ClipViewerDock)
    {
        if (ClipViewerDock->objectName().isEmpty())
            ClipViewerDock->setObjectName(QStringLiteral("ClipViewerDock"));
        ClipViewerDock->resize(400, 300);

        retranslateUi(ClipViewerDock);

        QMetaObject::connectSlotsByName(ClipViewerDock);
    } // setupUi

    void retranslateUi(QWidget *ClipViewerDock)
    {
        ClipViewerDock->setWindowTitle(QApplication::translate("ClipViewerDock", "Form", 0));
    } // retranslateUi

};

namespace Ui {
    class ClipViewerDock: public Ui_ClipViewerDock {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CLIPVIEWERDOCK_H
