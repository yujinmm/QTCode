#include "widget.h"
#include "ui_widget.h"
#include <QHostAddress>
#include <QFileDialog>
#include <QDebug>
#include <QFileInfo>
#include <QDataStream>


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    tcpSocket = NULL;
    //两个按钮dou不能按
    ui->Select_PBT->setEnabled(false);
    ui->Send_PBT->setEnabled(false);

    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, &QTcpSocket::connected,
           [=]()
           {
                ui->Messages_QTE->append("成功与服务器建立好连接");
                ui->Select_PBT->setEnabled(true);
           }
           );

    connect(tcpSocket, &QTcpSocket::disconnected,
           [=]()
           {
                ui->Messages_QTE->append("与服务器断开连接");
                ui->Select_PBT->setEnabled(false);
           }
           );
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_Connect_PBT_clicked()
{
    //获取服务器ip和端口
    QString ip = ui->IP_IE->text();
    //将Qstring 转换成 int
    quint16 Port = ui->Port_IE->text().toUInt();

    //主动和服务器建立
    tcpSocket->connectToHost(QHostAddress(ip),Port);
}

void Widget::on_Close_PBT_clicked()
{
    //主动和对方断开
    tcpSocket->disconnectFromHost();
    tcpSocket->close();
}

void Widget::on_Select_PBT_clicked()
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
        ui->Messages_QTE->append(filePath);

        ui->Select_PBT->setEnabled(false);
        ui->Send_PBT->setEnabled(true);
    }
    else
    {
        qDebug() << "选择文件路径出错 99";
        file.close();
    }

    QDataStream dataStream(&file);
    char *pbuf = new char[fileSize];

    memset(pbuf, 0, fileSize);
    dataStream.readRawData(pbuf, fileSize);

    //qDebug()输出存在char[],以16进制的方式
    QString strALL;
    for(int i = 0; i < fileSize; i++)
    {
        strALL += QString::asprintf("%02X",(uint8_t)pbuf[i]);
    }
    qDebug()<<strALL;
}

void Widget::on_Send_PBT_clicked()
{
//    QString head = QString("%1##%2").arg(fileName).arg(fileSize);

//    //发送头部信息
//    qint64 len = tcpSocket->write( head.toUtf8() );

//    if( len > 0 )
//    {
//        //发送真正的文件信息
//        //发送TCP粘包文件
//        //需要通过定时器
//        timer.start(20);
//    }
//    else
//    {
//        qDebug() << "头部信息发送失败 110";
//        file.close();
//        ui->Select_QPB->setEnabled(true);
//        ui->Send_QPB->setEnabled(false);
//    }
}
