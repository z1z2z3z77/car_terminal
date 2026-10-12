/********************************************************************************
** Form generated from reading UI file 'menu_widget.ui'
**
** Created by: Qt User Interface Compiler version 5.12.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MENU_WIDGET_H
#define UI_MENU_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_menu_widget
{
public:
    QGridLayout *gridLayout;
    QPushButton *btn_hardware;
    QLabel *label;
    QPushButton *btn_backup;
    QButtonGroup *buttonGroup;

    void setupUi(QWidget *menu_widget)
    {
        if (menu_widget->objectName().isEmpty())
            menu_widget->setObjectName(QString::fromUtf8("menu_widget"));
        menu_widget->resize(602, 394);
        gridLayout = new QGridLayout(menu_widget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        btn_hardware = new QPushButton(menu_widget);
        buttonGroup = new QButtonGroup(menu_widget);
        buttonGroup->setObjectName(QString::fromUtf8("buttonGroup"));
        buttonGroup->addButton(btn_hardware);
        btn_hardware->setObjectName(QString::fromUtf8("btn_hardware"));
        btn_hardware->setProperty("idx", QVariant(1));

        gridLayout->addWidget(btn_hardware, 1, 1, 1, 1);

        label = new QLabel(menu_widget);
        label->setObjectName(QString::fromUtf8("label"));
        QFont font;
        font.setPointSize(26);
        label->setFont(font);
        label->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label, 0, 0, 1, 2);

        btn_backup = new QPushButton(menu_widget);
        buttonGroup->addButton(btn_backup);
        btn_backup->setObjectName(QString::fromUtf8("btn_backup"));
        btn_backup->setProperty("idx", QVariant(2));

        gridLayout->addWidget(btn_backup, 1, 0, 1, 1);


        retranslateUi(menu_widget);

        QMetaObject::connectSlotsByName(menu_widget);
    } // setupUi

    void retranslateUi(QWidget *menu_widget)
    {
        menu_widget->setWindowTitle(QApplication::translate("menu_widget", "Form", nullptr));
        btn_hardware->setText(QApplication::translate("menu_widget", "\347\241\254\344\273\266\346\216\247\345\210\266", nullptr));
        label->setText(QApplication::translate("menu_widget", "\350\217\234\345\215\225", nullptr));
        btn_backup->setText(QApplication::translate("menu_widget", "\345\200\222\350\275\246\345\275\261\345\203\217", nullptr));
    } // retranslateUi

};

namespace Ui {
    class menu_widget: public Ui_menu_widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MENU_WIDGET_H
