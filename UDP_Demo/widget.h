#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QUdpSocket>    //UDP套接字

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void dealMsg();     //槽函数，处理对方发送过来的数据

private slots:
    void on_bindButton_clicked();

    void on_sendButton_clicked();

private:
    Ui::Widget *ui;

    QUdpSocket *udpSocket;
};
#endif // WIDGET_H
