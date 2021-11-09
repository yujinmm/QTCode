#include "widget.h"
#include "ui_widget.h"
#include "QHostAddress"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    //分配空间，指定父对象
    udpSocket = new QUdpSocket(this);

    //绑定
    qint16 temp = ui->local_port->text().toInt();
    udpSocket->bind(7777);

    //当对方发送数据过来
    //自动触发  reaadyRead()
    connect(udpSocket, &QUdpSocket::readyRead, this, &Widget::dealMsg);

}

//接收窗口
void Widget::dealMsg()
{
    //读取对方发送的内容
    char buf[1024] = { 0 };
    QHostAddress cliAddr;   //对方地址
    quint16 port;            //对方端口
    qint64 len = udpSocket->readDatagram(buf, sizeof(buf), &cliAddr, &port);
    if(len > 0)
    {
        //格式化   [192.168.2.2:8888]aaaa
        QString str = QString("[%1:%2] %3")
                                            .arg(cliAddr.toString())
                                            .arg(port)
                                            .arg(buf);
        ui->messages->setText(str);

    }

}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_bindButton_clicked()
{

}

//发送按钮
void Widget::on_sendButton_clicked()
{
    //先获取对方的IP和端口
    QString ip = ui->remote_ip->text();
    qint16 port = ui->remote_port->text().toInt();

    //获取编辑区内容
    QString str = ui->send_data->toPlainText();

    //给指定的IP发送数据
    udpSocket->writeDatagram(str.toUtf8(), QHostAddress(ip), port);

}
