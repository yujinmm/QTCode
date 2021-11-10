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

    connect(tcpServer, &QTcpServer::newConnection,
                //"=" 函数体内可以使用 Lambda 所在范围内所有可见的局部变量（包括 Lambda 所在类的 this），并且是值传递方式（相当于编译器自动为我们按值传递了所有局部变量）。
                [=]()
                {
                    //取出建立好的连接的套接字
                    tcpSocket = tcpServer->nextPendingConnection();
                    //获取对方的IP和端口
                    QString ip = tcpSocket->peerAddress().toString();
                    quint16 port = tcpSocket->peerPort();

                    QString temp = QString("[%1;%2]:成功连接").arg(ip).arg(port);

                    ui->MessageWindow->setText(temp);

                    //取出指针后在对其进行操作
                    connect(tcpSocket, &QTcpSocket::readyRead,
                                [=]()
                                {
                                    //从通信套接字取出内容,与自己软件对接失败
                                    QByteArray array = tcpSocket->readAll();
                                    ui->MessageWindow->append(array);

                                }

                            );
                }
            );

}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_SendButton_clicked()
{
    if(tcpSocket == NULL)
    {
        return;
    }
    //获取编辑区内容
    QString str = ui->InputWindow->toPlainText();
    //给对方发送数据，使用通信套接字tcpSoket, 把QString转换成char*数组
    //成功与ASCII码对接上
    tcpSocket->write( str.toLocal8Bit().data() );
    //无法与ASCII码对接上
//    tcpSocket->write( str.toUtf8().data() );
}

void Widget::on_CloseButton_clicked()
{
    if(tcpSocket == NULL)
    {
        return;
    }
    //主动与客户端端口断开连接
    tcpSocket->disconnectFromHost();
    tcpSocket->close();
    tcpSocket = NULL;
}
