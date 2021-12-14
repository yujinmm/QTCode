#include "tcpclient.h"
#include "ui_tcpclient.h"
#include "QHostAddress"

TcpClient::TcpClient(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TcpClient)
{
    ui->setupUi(this);
    tcpSocket = NULL;

    tcpSocket = new QTcpSocket(this);

    connect(tcpSocket, &QTcpSocket::connected,
           [=]()
           {
                ui->Messages_QTE->append("成功与服务器建立好连接");
           }
           );

    connect(tcpSocket, &QTcpSocket::readyRead,
           [=]()
           {
                //获取对方发送的内容
                QByteArray array = tcpSocket->readAll();
                //追加到编辑区中
                ui->Messages_QTE->append(array.toHex());
           }
           );

    connect(tcpSocket, &QTcpSocket::disconnected,
           [=]()
           {
                //这里还存在一个问题，就是没有手动断开连接，当窗口关闭后 对象树释放socket后断开连接，还会进来一次，这是就会报错
                ui->Messages_QTE->append("与服务器断开连接!");
                tcpSocket->close();
           }
           );
}

TcpClient::~TcpClient()
{
    delete ui;
}

void TcpClient::on_connect_PB_clicked()
{
    //获取服务器ip和端口
    QString ip = ui->IP_lineEdit->text();
    //将Qstring 转换成 int
    quint16 Port = ui->Port_lineEdit->text().toUInt();

    //主动和服务器建立
    tcpSocket->connectToHost(QHostAddress(ip),Port);
}

void TcpClient::on_disconnect_PB_clicked()
{
    //主动和对方断开
    tcpSocket->disconnectFromHost();
    tcpSocket->close();
}

void TcpClient::on_clear_PB_clicked()
{
    ui->Messages_QTE->clear();
}

void TcpClient::on_Messages_QTE_textChanged()
{
    ui->Messages_QTE->moveCursor(QTextCursor::End);
}
