/********************************************************************************
** Form generated from reading UI file 'librarypanel.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LIBRARYPANEL_H
#define UI_LIBRARYPANEL_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QListView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LibraryPanel
{
public:
    QHBoxLayout *horizontalLayout;
    QListView *listView;

    void setupUi(QWidget *LibraryPanel)
    {
        if (LibraryPanel->objectName().isEmpty())
            LibraryPanel->setObjectName(QStringLiteral("LibraryPanel"));
        LibraryPanel->resize(459, 369);
        horizontalLayout = new QHBoxLayout(LibraryPanel);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        listView = new QListView(LibraryPanel);
        listView->setObjectName(QStringLiteral("listView"));
        listView->setContextMenuPolicy(Qt::CustomContextMenu);
        listView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        listView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        listView->setDragEnabled(true);
        listView->setSelectionMode(QAbstractItemView::MultiSelection);
        listView->setMovement(QListView::Snap);
        listView->setResizeMode(QListView::Adjust);
        listView->setLayoutMode(QListView::Batched);
        listView->setViewMode(QListView::IconMode);
        listView->setUniformItemSizes(true);

        horizontalLayout->addWidget(listView);


        retranslateUi(LibraryPanel);

        QMetaObject::connectSlotsByName(LibraryPanel);
    } // setupUi

    void retranslateUi(QWidget *LibraryPanel)
    {
        LibraryPanel->setWindowTitle(QString());
    } // retranslateUi

};

namespace Ui {
    class LibraryPanel: public Ui_LibraryPanel {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LIBRARYPANEL_H
