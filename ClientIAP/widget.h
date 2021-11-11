#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

#include <QTcpSocket>    //通信套接字
#include <QFile>
#include <QTimer>


#define POLYNOMIAL          0x1021  //CRC16
#define INITIAL_REMAINDER   0x0000
#define FINAL_XOR_VALUE     0x0000
typedef unsigned short width_t;
#define WIDTH (8 * sizeof(width_t))
#define TOPBIT (1 << (WIDTH - 1))
#define Ymodem_SOH          0x01
#define Ymodem_STX          0x02
#define Ymodem_EOT          0x04
#define Ymodem_ACK          0x06
#define Ymodem_NAK          0x15
#define Ymodem_CAN          0x18
#define Ymodem_C            0x43


QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_Connect_PBT_clicked();

    void on_Close_PBT_clicked();

    void on_Select_PBT_clicked();

    void on_Send_PBT_clicked();

private:
    Ui::Widget *ui;

    //使用指针就需要动态分配空间，使用变量则不用，两种方法都可以
    QTcpSocket *tcpSocket;   //通信

    QFile file;  //文件对象
    QString fileName;//文件名称
    quint64 fileSize;//文件大小
    quint64 sendSize;//已经发送文件的大小

    char* fileAllbuf;//文件原始数据

    //分包参数
    qint16 packetNum;
    qint16 packetSize;
    qint16 lastpacketSize;

    char startFrame[133];
    char stopFrame[133];
    char dataFrame[1029];


    //协议标志，当接收到C以及ANK后置位true
    bool YgetC_flag;
    bool YgetACK_flag;

    bool fileRead_flag;
    bool sending_flag;

    QTimer timer;//定时器，当连接建立好后，定时检测协议标志位，判断是否由数据请求
    QTimer timerPacketSend;
};
#endif // WIDGET_H

void crcInit(void);
width_t crcCompute(unsigned char * message, unsigned int nBytes);

