#include "mainwindow.h"

#include <QApplication>

#include "plog/Appenders/ColorConsoleAppender.h"
#include "plog/Formatters/TxtFormatter.h"
#include "plog/Init.h"
#include "plog/Log.h"
int main(int argc, char *argv[])
{
    // 定义一个静态的日志输出器对象，配置为"带颜色的控制台输出"，格式为文本
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;

    //plog::verbose: 设置日志级别为详细模式（记录所有级别的日志）
    //&consoleAppender: 将日志输出目标设置为刚才定义的控制台输出器
    plog::init(plog::verbose, &consoleAppender);
    // 打印一条 INFO 级别的日志，表示系统已初始化
    PLOGI << "System initialized";
    //初始化qt应用程序对象
    QApplication a(argc, argv);
    //定义MainWindow 类
    MainWindow w;
    // showMaximized() 会让窗口启动时直接最大化填充屏幕
    w.showMaximized();
    // exec() 会让程序开始运行并等待用户交互（点击、触摸等）
    // 这个函数会一直阻塞，直到窗口关闭或程序退出
    // return 将返回程序的退出状态码给操作系统
    return a.exec();
}
