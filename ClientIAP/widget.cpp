#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    tcpServer = NULL;
    tcpSocket = NULL;

    //监听套接字
    tcpServer = new QTcpServer(this);
    //绑定当前网卡所有IP地址
    tcpServer->listen(QHostAddress::Any,8888);


}

Widget::~Widget()
{
    delete ui;
}

