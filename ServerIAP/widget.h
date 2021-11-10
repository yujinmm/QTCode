#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpServer>    //监听套接字
#include <QTcpSocket>    //通信套接字
#include <QFile>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void sendData();//发送文件数据

private slots:
    void on_Select_QPB_clicked();

    void on_Send_QPB_clicked();

private:
    Ui::Widget *ui;

    //使用指针就需要动态分配空间，使用变量则不用，两种方法都可以
    QTcpServer *tcpServer;   //监听
    QTcpSocket *tcpSocket;   //通信

    QFile file;  //文件对象
    QString fileName;//文件名称
    qint64 fileSize;//文件大小
    qint64 sendSize;//已经发送文件的大小

    QTimer timer;//定时器
};
#endif // WIDGET_H
