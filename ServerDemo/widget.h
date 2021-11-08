#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpServer>    //监听套接字
#include <QTcpSocket>    //通信套接字


QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_SendButton_clicked();

private:
    Ui::Widget *ui;

    //使用指针就需要动态分配空间，使用变量则不用，两种方法都可以
    QTcpServer *tcpServer;   //监听
    QTcpSocket *tcpSocket;   //通信
};
#endif // WIDGET_H
