/********************************************************************************
** Form generated from reading UI file 'hare_widget.ui'
**
** Created by: Qt User Interface Compiler version 5.12.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_HARE_WIDGET_H
#define UI_HARE_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_hare_widget
{
public:
    QLabel *label;
    QLabel *label_2;
    QPushButton *pushButton;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QPushButton *btn_led_on;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *btn_led_off;
    QSpacerItem *horizontalSpacer_3;
    QWidget *layoutWidget1;
    QFormLayout *formLayout;
    QLabel *label_3;
    QLabel *label_ps;
    QLabel *label_4;
    QLabel *label_als;
    QLabel *label_5;
    QLabel *label_ir;

    void setupUi(QWidget *hare_widget)
    {
        if (hare_widget->objectName().isEmpty())
            hare_widget->setObjectName(QString::fromUtf8("hare_widget"));
        hare_widget->resize(684, 407);
        label = new QLabel(hare_widget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 20, 641, 71));
        QFont font;
        font.setPointSize(26);
        label->setFont(font);
        label->setAlignment(Qt::AlignCenter);
        label_2 = new QLabel(hare_widget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(30, 190, 611, 51));
        label_2->setFont(font);
        label_2->setAlignment(Qt::AlignCenter);
        pushButton = new QPushButton(hare_widget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(300, 360, 101, 31));
        layoutWidget = new QWidget(hare_widget);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(30, 100, 611, 61));
        horizontalLayout_2 = new QHBoxLayout(layoutWidget);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        btn_led_on = new QPushButton(layoutWidget);
        btn_led_on->setObjectName(QString::fromUtf8("btn_led_on"));
        QFont font1;
        font1.setPointSize(18);
        btn_led_on->setFont(font1);

        horizontalLayout_2->addWidget(btn_led_on);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        btn_led_off = new QPushButton(layoutWidget);
        btn_led_off->setObjectName(QString::fromUtf8("btn_led_off"));
        btn_led_off->setFont(font1);

        horizontalLayout_2->addWidget(btn_led_off);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_3);

        layoutWidget1 = new QWidget(hare_widget);
        layoutWidget1->setObjectName(QString::fromUtf8("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(30, 260, 611, 81));
        formLayout = new QFormLayout(layoutWidget1);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setContentsMargins(0, 0, 0, 0);
        label_3 = new QLabel(layoutWidget1);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label_3);

        label_ps = new QLabel(layoutWidget1);
        label_ps->setObjectName(QString::fromUtf8("label_ps"));

        formLayout->setWidget(0, QFormLayout::FieldRole, label_ps);

        label_4 = new QLabel(layoutWidget1);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_4);

        label_als = new QLabel(layoutWidget1);
        label_als->setObjectName(QString::fromUtf8("label_als"));

        formLayout->setWidget(1, QFormLayout::FieldRole, label_als);

        label_5 = new QLabel(layoutWidget1);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_5);

        label_ir = new QLabel(layoutWidget1);
        label_ir->setObjectName(QString::fromUtf8("label_ir"));

        formLayout->setWidget(2, QFormLayout::FieldRole, label_ir);


        retranslateUi(hare_widget);

        QMetaObject::connectSlotsByName(hare_widget);
    } // setupUi

    void retranslateUi(QWidget *hare_widget)
    {
        hare_widget->setWindowTitle(QApplication::translate("hare_widget", "Form", nullptr));
        label->setText(QApplication::translate("hare_widget", "Light control", nullptr));
        label_2->setText(QApplication::translate("hare_widget", "environment percetion", nullptr));
        pushButton->setText(QApplication::translate("hare_widget", "BACK", nullptr));
        btn_led_on->setText(QApplication::translate("hare_widget", "on", nullptr));
        btn_led_off->setText(QApplication::translate("hare_widget", "off", nullptr));
        label_3->setText(QApplication::translate("hare_widget", "PS:", nullptr));
        label_ps->setText(QApplication::translate("hare_widget", "0.0", nullptr));
        label_4->setText(QApplication::translate("hare_widget", "ALS:", nullptr));
        label_als->setText(QApplication::translate("hare_widget", "0.0", nullptr));
        label_5->setText(QApplication::translate("hare_widget", "IR:", nullptr));
        label_ir->setText(QApplication::translate("hare_widget", "0.0", nullptr));
    } // retranslateUi

};

namespace Ui {
    class hare_widget: public Ui_hare_widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HARE_WIDGET_H
