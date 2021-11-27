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
//以下为自定义内容
//标志当前设备在线，在配置模式下，有设备连接，间隔时间向上位机发送
#define Ymodem_Configure       0x1B
#define Ymodem_Setting       0x1F


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

    void on_clc1_PBT_clicked();

    void on_clc2_PBT_clicked();

    void on_setting_PBT_clicked();

    void on_goBack_PBT_clicked();

    void on_upgrade_PBT_clicked();

    void on_readDev_PBT_clicked();

    void on_changeDev_PBT_clicked();

    void on_goto_PBT_clicked();

    void on_start_PBT_clicked();

    void on_stop_PBT_clicked();

    void on_rst_PBT_clicked();

private:
    Ui::Widget *ui;

    //使用指针就需要动态分配空间，使用变量则不用，两种方法都可以
    QTcpSocket *tcpSocket;   //通信

    QFile file;  //文件对象
    QString fileName;//文件名称
    quint64 fileSize;//文件大小

    char fileAllbuf[50000];//文件原始数据

    //分包参数
    qint16 packetNum;
    qint16 packetSize;
    qint16 lastpacketSize;

    //需要发送的Ymodem起始帧，包含文件名以及文件大小
    char startFrame[133];
    //需要发送的Ymodem停止帧
    char stopFrame[133];
    //需要发送的Ymodem数据帧
    char dataFrame[1029];
    //需要发送的自定义的14字节命令帧
    char commandFrame[14];
    //接收到自定义的14字节命令帧
    quint8 commandRXframe[14];

    quint8 mac[6] = {0x00,0x08,0xdc,0x11,0x11,0x18};

    quint8 local_ip[4] = {192,168,0,116};

    quint8 gateway[4] = {192,168,0,1};

    //标志当前.bin文件发送阶段:1.首帧 2.数据帧 3.结束帧
    quint16 dataSendstage;

    //标志设备当前状态的标志:1.顶层模式 2.信息修改模式 3.固件升级模式
    quint16 devstage;


    //协议标志，当接收到C以及ANK后置位true
    bool YgetC_flag;
    bool YgetACK_flag;

    QTimer timer;//定时器，当连接建立好后，定时检测协议标志位，判断是否由数据请求
    QTimer timerPacketSend;
};
#endif // WIDGET_H

void crcInit(void);
width_t crcCompute(unsigned char * message, unsigned int nBytes);

