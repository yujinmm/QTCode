#include "clientwidget.h"
#include "ui_clientwidget.h"
#include "QHostAddress"

ClientWidget::ClientWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClientWidget)
{
    ui->setupUi(this);

    tcpSocket = NULL;

    tcpSocket = new QTcpSocket(this);

    connect(tcpSocket, &QTcpSocket::connected,
           [=]()
           {
                ui->Message_Window->setText("成功与服务器建立好连接");
           }
           );

    connect(tcpSocket, &QTcpSocket::readyRead,
           [=]()
           {
                //获取对方发送的内容
                QByteArray array = tcpSocket->readAll();
                //追加到编辑区中
                ui->Message_Window->append(array.toHex());
           }
           );
}

ClientWidget::~ClientWidget()
{
    delete ui;
}

void ClientWidget::on_pushButton_Connect_clicked()
{
    //获取服务器ip和端口
    QString ip = ui->lineEdit_IP->text();
    //将Qstring 转换成 int
    quint16 Port = ui->lineEdit_PORT->text().toUInt();

    //主动和服务器建立
    tcpSocket->connectToHost(QHostAddress(ip),Port);
}

void ClientWidget::on_pushButton_Send_clicked()
{
    //获取编辑框内容
    QString str = ui->Send_Window->toPlainText();
    //发送数据
    tcpSocket->write( str.toLocal8Bit().data() );
}

void ClientWidget::on_pushButton_Close_clicked()
{
    //主动和对方断开
    tcpSocket->disconnectFromHost();
    tcpSocket->close();
}
