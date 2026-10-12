#include "menu_widget.h"
#include "ui_menu_widget.h"
#include "common.h"

#include "plog/Log.h"

#include "fcntl.h"
#include "linux/input.h"
#include "unistd.h"


menu_widget::menu_widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::menu_widget)
{
    ui->setupUi(this);
    // 初始化由 Qt Designer 生成的 UI 界面
    // 逻辑：当界面上属于 buttonGroup 的任意按钮被点击 -> 触发本类的 on_buttonGroup_buttonClicked 函数
    connect(ui->buttonGroup,
            static_cast<void (QButtonGroup::*)(QAbstractButton *)>(&QButtonGroup::buttonClicked), this,
            &menu_widget::on_buttonGroup_buttonClicked);
    // 初始化板载物理按键监听（开启一个后台线程）
    //this->initBoardButton();
}

menu_widget::~menu_widget()
{
    delete ui;
     if(this->btn_fd >= 0)
        ::close(this->btn_fd); 
}
void menu_widget::on_buttonGroup_buttonClicked(QAbstractButton *button)
{   // 类型转换：将通用的 QAbstractButton 指针转为 QPushButton 指针
    QPushButton *btn = dynamic_cast<QPushButton *>(button);
    // 从按钮的属性中提取索引值 idx
    int idx = btn->property("idx").toInt();
    // 发射信号 changePage，将提取到的索引值 idx 作为参数传递
    emit changePage(idx);
}

void menu_widget::initBoardButton()
{   // 打开板载物理按键监听的设备文件 /dev/input/event2
    // 以只读模式打开，返回文件描述符 btn_fd
    this->btn_fd = open("/dev/input/event2", O_RDONLY);
    // 检查文件描述符是否成功打开
    if (this->btn_fd < 0)
    {
        PLOG_FATAL << "open KEY0 failed";
        std::exit(1);
    }
 
    //创建监听线程
    this->btn_thread = std::unique_ptr<std::thread>(new std::thread(
        // Lambda 表达式，捕获 this 指针以便在线程内访问类成员
        [this]()
        {
            while (1)
            {   // Linux 标准输入事件结构体
                input_event ev; 
                // 读取事件 
                //程序在这里暂停，直到内核有按键事件发生
                if (read(this->btn_fd, &ev, sizeof(ev)) != sizeof(ev))
                {
                    PLOG_FATAL << "read KEY0 event failed";
                    continue;
                }
                //  解析事件
                // EV_KEY: 事件类型是按键
                // KEY_0: 按键码是 KEY_0 (对应板子上的某个特定物理按键)
                // value == 1: 表示按键"按下" (0是松开，2是长按)    
                if (ev.type == EV_KEY && ev.code == KEY_0 && ev.value == 1)
                {   // 触发动作
                    // 当检测到物理按键 KEY0 按下时，发送信号切换回菜单首页 (PageEnum::MenuPage = 0)
                    // 这相当于手机的 "Home" 键功能
                    emit changePage(static_cast<int>(PageEnum::MenuPage));
                    PLOGI << "Button 0 pressed";
                }
            }
        }));
    //分离线程
    // 让线程在后台独立运行，不需要主线程等待它结束
    this->btn_thread->detach();
}
