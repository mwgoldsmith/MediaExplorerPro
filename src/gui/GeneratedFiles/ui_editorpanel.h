/********************************************************************************
** Form generated from reading UI file 'editorpanel.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EDITORPANEL_H
#define UI_EDITORPANEL_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_EditorPanel
{
public:
    QHBoxLayout *horizontalLayout;
    QLabel *label;

    void setupUi(QWidget *EditorPanel)
    {
        if (EditorPanel->objectName().isEmpty())
            EditorPanel->setObjectName(QStringLiteral("EditorPanel"));
        EditorPanel->resize(400, 300);
        horizontalLayout = new QHBoxLayout(EditorPanel);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(EditorPanel);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);


        retranslateUi(EditorPanel);

        QMetaObject::connectSlotsByName(EditorPanel);
    } // setupUi

    void retranslateUi(QWidget *EditorPanel)
    {
        EditorPanel->setWindowTitle(QApplication::translate("EditorPanel", "Form", 0));
        label->setText(QApplication::translate("EditorPanel", "editor", 0));
    } // retranslateUi

};

namespace Ui {
    class EditorPanel: public Ui_EditorPanel {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EDITORPANEL_H
