#ifndef BACKUPVIEW_H
#define BACKUPVIEW_H

#include <QWidget>
#include <QTimer>
#include <memory>
#include "V4L2Camera.h"

namespace Ui {
class backupview;
}

class backupview : public QWidget
{
    Q_OBJECT

public:
    explicit backupview(QWidget *parent = nullptr);
    ~backupview();

signals:
    void goBack();

private slots:
    void on_btn_start_clicked(); 
    void on_btn_stop_clicked();
    void on_btn_info_clicked();
    void on_btn_back_clicked();
    
private:
    Ui::backupview *ui;
    std::unique_ptr<V4L2Camera> camera;
    std::unique_ptr<QTimer> update_img_timer;
};

#endif // BACKUPVIEW_H
