#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>
#include <QDebug>
#include <QFileInfo>
#include <QDataStream>

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
    setWindowTitle("服务器端口位：8888");

    //两个按钮dou不能按
    ui->Select_QPB->setEnabled(false);
    ui->Send_QPB->setEnabled(false);

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

                    ui->Message_QTE->setText(temp);

//                    //取出指针后在对其进行操作
//                    connect(tcpSocket, &QTcpSocket::readyRead,
//                                [=]()
//                                {
//                                    //从通信套接字取出内容,与自己软件对接失败
//                                    QByteArray array = tcpSocket->readAll();
//                                    ui->Message_QTE->append(array);

//                                }

//                            );
                    //成功连接后，才能选择文件
                    ui->Select_QPB->setEnabled(true);
                }
            );

    connect(&timer, &QTimer::timeout,
            [=]()
            {
                //关闭定时器
                timer.stop();

                //发送文件
                sendData();
            }


            );

}

Widget::~Widget()
{
    delete ui;
}

//选择文件按钮
void Widget::on_Select_QPB_clicked()
{
    //利用文件对话框获取文件路径
    QString filePath = QFileDialog::getOpenFileName(this, "open", "../");
    if(false == filePath.isEmpty())   //如果选择文件路径有效
    {
        fileName.clear();
        fileSize = 0;

        //利用QFileInfo对象，获取文件信息，需要文件路径
        QFileInfo info(filePath);
        fileName = info.fileName();
        fileSize = info.size();

        qDebug() << fileName <<fileSize;
        sendSize = 0;

        //只读方式打开文件
        //指定文件的名字
        file.setFileName(filePath);

        //打开文件
        bool isOK = file.open(QIODevice::ReadOnly);
        if(isOK == false)
        {
            qDebug() << "只读方式打开文件失败 88";
        }

        //提示文件打开的路径
        ui->Message_QTE->append(filePath);

        ui->Select_QPB->setEnabled(false);
        ui->Send_QPB->setEnabled(true);
    }
    else
    {
        qDebug() << "选择文件路径出错 112";
        file.close();
    }

//    QDataStream dataStream(&file);
//    char *pbuf = new char[fileSize];
//    memset(pbuf, 0, fileSize);
//    dataStream.readRawData(pbuf, fileSize);

//    QString strALL;
//    for(int i = 0; i < fileSize; i++)
//    {
//        strALL += QString::asprintf("%02X ",(uint8_t)pbuf[i]);
//    }
//    qDebug()<<strALL;

}

//发送文件按钮
void Widget::on_Send_QPB_clicked()
{
    QString head = QString("%1##%2").arg(fileName).arg(fileSize);

    //发送头部信息
    qint64 len = tcpSocket->write( head.toUtf8() );

    if( len > 0 )
    {
        //发送真正的文件信息
        //发送TCP粘包文件
        //需要通过定时器
        timer.start(20);
    }
    else
    {
        qDebug() << "头部信息发送失败 110";
        file.close();
        ui->Select_QPB->setEnabled(true);
        ui->Send_QPB->setEnabled(false);
    }
}

void Widget::sendData()
{
    qint64 len = 0;
    do
    {
        //每次发送数据的大小
        char buf[4*1024] = {0};
        len = 0;

        //往文件中读数据
        len = file.read(buf, sizeof(buf));

        qDebug() << buf;

        //发送数据，都多少，发多少
        len = tcpSocket->write(buf, len);

        //发送的数据需要累加
        sendSize += len;

    }while( len > 0 );

    if( sendSize == fileSize)
    {
         ui->Message_QTE->append("文件发送完毕");
         file.close();

         //把客户端断开
         tcpSocket->disconnectFromHost();
         tcpSocket->close();
    }
}
