#include "hare_widget.h"
#include "ui_hare_widget.h"
#include "plog/Log.h"

#include "fcntl.h"
#include "unistd.h"
#include <cstdlib>
#include <memory>
#include <QString>

hare_widget::hare_widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::hare_widget)
{
    ui->setupUi(this);
    this->led = std::unique_ptr<LED>(new LED("/dev/led"));
    this->led->on();
     try
    {
        this->ap3216c = std::unique_ptr<AP3216C>(new AP3216C("/dev/ap3216c"));
    }
    catch (const std::exception &e)
    {   //初始化失败打印错误日志，将指针置为 nullptr
        PLOGE << "Failed to initialize AP3216C: " << e.what();
        this->ap3216c = nullptr;
    }
    this->sensor_timer = new QTimer(this);
    connect(this->sensor_timer, &QTimer::timeout, this, &hare_widget::updateSensorData);

    this->sensor_timer->start(500);
}

hare_widget::~hare_widget()
{   
    if(this->sensor_timer)
    {
       this->sensor_timer->stop();
       delete this->sensor_timer;
    }
    delete ui;
}
void hare_widget::updateSensorData()
{
    // 检查 AP3216C 传感器是否初始化成功且已打开
    if (!this->ap3216c || !this->ap3216c->isOpen())
    {
        return;
    }
    // 调用封装好的 readData() 方法，从内核驱动读取 IR、ALS、PS 三组数据
    SensorData data = this->ap3216c->readData();
    
    // 更新 UI 标签
    // QString::number() 将整数转换为字符串
    // setText() 将字符串显示到界面对应的 Label 控件上
    ui->label_ps->setText(QString::number(data.ps_data));   // 显示接近距离 (Proximity)
    ui->label_als->setText(QString::number(data.als_data)); // 显示环境光强度 (Ambient Light)
    ui->label_ir->setText(QString::number(data.ir_data));   // 显示红外强度 (Infrared)
}
void hare_widget::on_btn_led_on_clicked()
{
    this->led->on();
}

void hare_widget::on_btn_led_off_clicked()
{
    this->led->off();
}

void hare_widget::on_pushButton_clicked()
{
    emit goBack();
}
