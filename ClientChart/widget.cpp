#include "widget.h"
#include "ui_widget.h"
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    //线程部分
        //动态分配空间,不能指定父对象
        mythrd_1 = new myThread(1,Xcount);
        //创建子线程
        thread_1 = new QThread(this);
        thread_1->start();
        //把我自定义的线程加入到子线程中
        mythrd_1->moveToThread(thread_1);

        //主线程读取数据，并出发与一个与子线程链接的信号
        connect(ui->client_my->tcpSocket, &QTcpSocket::readyRead, this, &Widget::readsocket);
        //将数据传递给子线程,线程传参
        connect(this, &Widget::startDeal1, mythrd_1, &myThread::dealData);
        //关闭窗口的同时关闭线程
        connect(this, &Widget::destroyed, this, &Widget::closeThread);

        connect(mythrd_1, &myThread::startprint, this, &Widget::dealTimeout);

        bytarr1.resize(400);
        bytarr.resize(1000);
}

Widget::~Widget()
{
    delete ui;
}

//读取网络数据，分包，并发送一个与子线程连接的信号
void Widget::readsocket()
{
    bytarr = ui->client_my->tcpSocket->readAll();
    qDebug() << bytarr;
    for(int i = 0 ; i < 400 ; i++)
    {
        bytarr1[i] = bytarr.at(i+6);
//        qDebug() << bytarr1.toHex();
    }
    //    qDebug() << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz");
    //    qDebug() <<bytarr.size();
    //    qDebug() << "主线程号:" << QThread::currentThread();
    emit startDeal1(1,bytarr1);
}

//关闭窗口后出发该槽函数关闭线程
void Widget::closeThread()
{
    thread_1->quit();
    thread_1->wait();
}

//定时刷新数据图表
void Widget::dealTimeout(int i , QVector<QPointF> v)
{
    if(i==1)
    {
        ui->chart_my->series->replace(v);
    }
}
