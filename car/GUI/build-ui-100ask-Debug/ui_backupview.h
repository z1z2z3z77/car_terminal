/********************************************************************************
** Form generated from reading UI file 'backupview.ui'
**
** Created by: Qt User Interface Compiler version 5.12.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BACKUPVIEW_H
#define UI_BACKUPVIEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_backupview
{
public:
    QLabel *image_label;
    QWidget *layoutWidget;
    QFormLayout *formLayout;
    QPushButton *btn_start;
    QPushButton *btn_stop;
    QPushButton *btn_info;
    QPushButton *btn_back;

    void setupUi(QWidget *backupview)
    {
        if (backupview->objectName().isEmpty())
            backupview->setObjectName(QString::fromUtf8("backupview"));
        backupview->resize(575, 425);
        image_label = new QLabel(backupview);
        image_label->setObjectName(QString::fromUtf8("image_label"));
        image_label->setGeometry(QRect(30, 20, 401, 351));
        image_label->setStyleSheet(QString::fromUtf8(""));
        image_label->setFrameShape(QFrame::Box);
        layoutWidget = new QWidget(backupview);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(470, 20, 97, 351));
        formLayout = new QFormLayout(layoutWidget);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setContentsMargins(0, 0, 0, 0);
        btn_start = new QPushButton(layoutWidget);
        btn_start->setObjectName(QString::fromUtf8("btn_start"));

        formLayout->setWidget(0, QFormLayout::LabelRole, btn_start);

        btn_stop = new QPushButton(layoutWidget);
        btn_stop->setObjectName(QString::fromUtf8("btn_stop"));

        formLayout->setWidget(1, QFormLayout::LabelRole, btn_stop);

        btn_info = new QPushButton(layoutWidget);
        btn_info->setObjectName(QString::fromUtf8("btn_info"));

        formLayout->setWidget(2, QFormLayout::LabelRole, btn_info);

        btn_back = new QPushButton(layoutWidget);
        btn_back->setObjectName(QString::fromUtf8("btn_back"));

        formLayout->setWidget(3, QFormLayout::LabelRole, btn_back);


        retranslateUi(backupview);

        QMetaObject::connectSlotsByName(backupview);
    } // setupUi

    void retranslateUi(QWidget *backupview)
    {
        backupview->setWindowTitle(QApplication::translate("backupview", "Form", nullptr));
        image_label->setText(QString());
        btn_start->setText(QApplication::translate("backupview", "start", nullptr));
        btn_stop->setText(QApplication::translate("backupview", "stop", nullptr));
        btn_info->setText(QApplication::translate("backupview", "info", nullptr));
        btn_back->setText(QApplication::translate("backupview", "back", nullptr));
    } // retranslateUi

};

namespace Ui {
    class backupview: public Ui_backupview {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BACKUPVIEW_H
