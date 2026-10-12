#ifndef MENU_WIDGET_H
#define MENU_WIDGET_H

#include <QWidget>
#include <QAbstractButton>

#include <thread>
#include <memory>
namespace Ui {
class menu_widget;
}

class menu_widget : public QWidget
{
    Q_OBJECT

public:
    explicit menu_widget(QWidget *parent = nullptr);
    ~menu_widget();
    void on_buttonGroup_buttonClicked(QAbstractButton *button);
    
signals:
    void changePage(int idx);

private:
    Ui::menu_widget *ui;
    void initBoardButton();
    int btn_fd;
    std::unique_ptr<std::thread> btn_thread;
};

#endif // MENU_WIDGET_H
