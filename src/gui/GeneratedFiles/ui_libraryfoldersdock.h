/********************************************************************************
** Form generated from reading UI file 'libraryfoldersdock.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LIBRARYFOLDERSDOCK_H
#define UI_LIBRARYFOLDERSDOCK_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LibraryFoldersDock
{
public:
    QHBoxLayout *horizontalLayout;
    QTreeWidget *foldersTreeWidget;

    void setupUi(QWidget *LibraryFoldersDock)
    {
        if (LibraryFoldersDock->objectName().isEmpty())
            LibraryFoldersDock->setObjectName(QStringLiteral("LibraryFoldersDock"));
        LibraryFoldersDock->resize(216, 440);
        horizontalLayout = new QHBoxLayout(LibraryFoldersDock);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        foldersTreeWidget = new QTreeWidget(LibraryFoldersDock);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QStringLiteral("1"));
        foldersTreeWidget->setHeaderItem(__qtreewidgetitem);
        foldersTreeWidget->setObjectName(QStringLiteral("foldersTreeWidget"));
        foldersTreeWidget->setUniformRowHeights(true);
        foldersTreeWidget->setColumnCount(1);
        foldersTreeWidget->header()->setVisible(false);

        horizontalLayout->addWidget(foldersTreeWidget);


        retranslateUi(LibraryFoldersDock);

        QMetaObject::connectSlotsByName(LibraryFoldersDock);
    } // setupUi

    void retranslateUi(QWidget *LibraryFoldersDock)
    {
        LibraryFoldersDock->setWindowTitle(QApplication::translate("LibraryFoldersDock", "Form", 0));
    } // retranslateUi

};

namespace Ui {
    class LibraryFoldersDock: public Ui_LibraryFoldersDock {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LIBRARYFOLDERSDOCK_H
