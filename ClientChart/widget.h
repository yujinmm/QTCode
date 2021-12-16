#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "mythread.h"
#include "tcpclient.h"


QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    Ui::Widget *ui;

    //读取socket数据
    void readsocket();

    //关闭窗口同时关闭线程
    void closeThread();

    //定时刷新数据图标的槽函数
    void dealTimeout(int i , QVector<QPointF> v);

signals:
    void startDeal1(int i , QByteArray byteArray);

private:


    myThread *mythrd_1;
    QThread *thread_1;

    QByteArray bytarr;

    QByteArray bytarr1;

    TcpClient * myClient;
};
#endif // WIDGET_H
