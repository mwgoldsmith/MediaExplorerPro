/********************************************************************************
** Form generated from reading UI file 'clippropertiesdock.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CLIPPROPERTIESDOCK_H
#define UI_CLIPPROPERTIESDOCK_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ClipPropertiesDock
{
public:
    QHBoxLayout *horizontalLayout;
    QTreeWidget *treeWidget;

    void setupUi(QWidget *ClipPropertiesDock)
    {
        if (ClipPropertiesDock->objectName().isEmpty())
            ClipPropertiesDock->setObjectName(QStringLiteral("ClipPropertiesDock"));
        ClipPropertiesDock->resize(400, 300);
        horizontalLayout = new QHBoxLayout(ClipPropertiesDock);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        treeWidget = new QTreeWidget(ClipPropertiesDock);
        treeWidget->setObjectName(QStringLiteral("treeWidget"));
        treeWidget->setUniformRowHeights(true);
        treeWidget->setItemsExpandable(false);
        treeWidget->setAllColumnsShowFocus(true);
        treeWidget->setColumnCount(2);

        horizontalLayout->addWidget(treeWidget);


        retranslateUi(ClipPropertiesDock);

        QMetaObject::connectSlotsByName(ClipPropertiesDock);
    } // setupUi

    void retranslateUi(QWidget *ClipPropertiesDock)
    {
        ClipPropertiesDock->setWindowTitle(QApplication::translate("ClipPropertiesDock", "Form", 0));
        QTreeWidgetItem *___qtreewidgetitem = treeWidget->headerItem();
        ___qtreewidgetitem->setText(1, QApplication::translate("ClipPropertiesDock", "Value", 0));
        ___qtreewidgetitem->setText(0, QApplication::translate("ClipPropertiesDock", "Property", 0));
    } // retranslateUi

};

namespace Ui {
    class ClipPropertiesDock: public Ui_ClipPropertiesDock {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CLIPPROPERTIESDOCK_H
