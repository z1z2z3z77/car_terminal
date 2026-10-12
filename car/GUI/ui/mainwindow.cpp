#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "menu_widget.h"
#include "common.h"
#include "plog/Log.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(static_cast<int>(PageEnum::MenuPage));

     connect(ui->page_menu, &menu_widget::changePage, this, &MainWindow::onChangePage);
    // 当 backup页面的 goBack 信号触发时 -> 执行 lambda 表达式
    // 在 MainWindow 构造函数中添加或修改这一行
    connect(ui->page_backupview, &backupview::goBack, this, [=](){
    // 切换回主菜单页面 (使用您代码中定义的枚举更规范)
    ui->stackedWidget->setCurrentIndex(static_cast<int>(PageEnum::MenuPage));
}); //hardwarewidget切换回主菜单页面
    connect(ui->page_hardware, &hare_widget::goBack, this, [=](){
        ui->stackedWidget->setCurrentIndex(static_cast<int>(PageEnum::MenuPage));
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onChangePage(int idx)
{   //打印日志  记录跳转请求的目标索引，方便调试
    PLOGI << "onChangePage: " << idx;
    //参数合法性检查
    if (idx < static_cast<int>(PageEnum::MenuPage) || idx > static_cast<int>(PageEnum::BackupviewPage))
    {  //如果目标索引超出范围，打印错误日志并返回
        PLOGF << "invalid page";
        return;
    }
    //更新当前显示的页面
    ui->stackedWidget->setCurrentIndex(idx);
}
