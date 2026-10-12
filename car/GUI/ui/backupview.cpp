#include "backupview.h"
#include "ui_backupview.h"
#include "plog/Log.h"

backupview::backupview(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::backupview)
{
    ui->setupUi(this);
    this->update_img_timer = std::unique_ptr<QTimer>(new QTimer(this));
    this->update_img_timer->setInterval(500);
    //this->camera = std::unique_ptr<V4L2Camera>(new V4L2Camera("/dev/video1", 640, 480, 15, 3));

    connect(this->update_img_timer.get(), &QTimer::timeout, this, [&](){
        PLOGD << "Timer triggered...";
        if(this->camera == nullptr)
            return;
        FrameData frame_data = this->camera->getFrame();
        if(frame_data.yuyv_data.get() == nullptr)
        {   PLOGW << "Frame queue is empty!";
            return;
        }
         PLOGI << "Got a frame! Size: " << frame_data.length << " bytes";
        QImage img;
    // 使用我们在第一步中新增的 frame_data.length
    bool success = img.loadFromData(frame_data.yuyv_data.get(), frame_data.length, "JPG");
        if (success) {
        // 缩放并显示 (保持您原有的逻辑)
        auto resized_img = img.scaled(360, 272, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        this->ui->image_label->setPixmap(QPixmap::fromImage(resized_img)); //贴到ui界面的img_label标签显示出来
    } else {
        PLOGE << "Failed to decode MJPEG frame!";
    }
    });
}

backupview::~backupview()
{
    delete ui;
    
}

void backupview::on_btn_start_clicked()
{
    // 当用户用手点击了 Start 按钮时，系统早就稳定了，此时再打开摄像头   
    try { 
        if (this->camera == nullptr) {
            PLOGI << "正在延迟初始化摄像头...";
            this->camera = std::unique_ptr<V4L2Camera>(new V4L2Camera("/dev/video1", 640, 480, 15, 3)); 
            }
            this->camera->startCapture();
             this->update_img_timer->start(); 
             PLOGI << "摄像头启动成功！"; 
             } catch (const std::exception& e) 
             { PLOGE << "摄像头启动失败: " << e.what();}
}

void backupview::on_btn_stop_clicked()
{
    this->camera->stopCapture();
    this->update_img_timer->stop();
}

void backupview::on_btn_info_clicked()
{
     this->camera->getParameters();
}

void backupview::on_btn_back_clicked()
{
    this->camera->stopCapture();
    this->update_img_timer->stop();
    emit goBack();
}
