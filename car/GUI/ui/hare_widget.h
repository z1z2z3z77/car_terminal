#ifndef HARE_WIDGET_H
#define HARE_WIDGET_H
#include <QTimer>
#include <QWidget>

#include <memory>
#include "led.h"
#include "ap3216c.h"
namespace Ui {
class hare_widget;
}

class hare_widget : public QWidget
{
    Q_OBJECT

public:
    explicit hare_widget(QWidget *parent = nullptr);
    ~hare_widget();
    void updateSensorData();
signals:
    void goBack();
private slots:
    void on_btn_led_on_clicked();

    void on_btn_led_off_clicked();

    void on_pushButton_clicked();

private:
    Ui::hare_widget *ui;
    std::unique_ptr<LED> led;
    std::unique_ptr<AP3216C> ap3216c;
    QTimer *sensor_timer;
};

#endif // HARE_WIDGET_H
