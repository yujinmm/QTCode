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
    this->setFixedSize(464,465);

    //初始化CRC
    crcInit();

    //
    QStringList strlist;
    strlist<<"      ";
    strlist<<"IAP模式";
    strlist<<"APP模式";

    ui->comboBox->addItems(strlist);
    //防止野指针
    tcpSocket = NULL;

    //.bin文件发送阶段，初始化为
    dataSendstage = 1;
    //默认位顶层模式
    devstage = 1;

    //两个按钮都不能按
    ui->Select_PBT->setEnabled(false);
    ui->Send_PBT->setEnabled(false);

    //接收到Ymodem协议后置位
    YgetC_flag = 0;
    YgetACK_flag = 0;

    //分包参数
    packetNum = 0;
    packetSize = 1024;
    lastpacketSize = 0;

    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, &QTcpSocket::connected,
           [=]()
           {
                ui->Messages_QTE->append("成功与服务器建立好连接!");
                ui->Select_PBT->setEnabled(true);
                ui->label_State->setText("已连接");
                //当网络连接状态更改，需要初始化.bin文件发送的标志位
                dataSendstage = 1;
                YgetC_flag = 0;
                YgetACK_flag = 0;
           }
           );

    connect(tcpSocket, &QTcpSocket::disconnected,
           [=]()
           {
                //这里还存在一个问题，就是没有手动断开连接，当窗口关闭后 对象树释放socket后断开连接，还会进来一次，这是就会报错
                ui->Messages_QTE->append("与服务器断开连接!");
                ui->Select_PBT->setEnabled(false);
                ui->label_State->setText("未连接");
                ui->label_DevS->setText("下位机当前状态");
                //当网络连接状态更改，需要初始化.bin文件发送的标志位
                dataSendstage = 1;
                YgetC_flag = 0;
                YgetACK_flag = 0;
           }
           );

    connect(tcpSocket, &QTcpSocket::readyRead,
           [=]()
           {
                //获取对方发送的内容
                QByteArray array = tcpSocket->readAll();
                //追加到编辑区中
                ui->revMessage_QTE->append(array.toHex());

                if(array.size() == 1)
                {
                    if( array.at(0) == Ymodem_Configure )
                    {
                        ui->label_DevS->setText("顶层模式");
                        devstage = 1;
                        ui->Dev_IP->clear();
                        ui->Dev_Gateway->clear();
                        ui->Dev_Mac->clear();
                        ui->comboBox->setCurrentIndex(0);
                    }
                    else if( array.at(0) == Ymodem_Setting )
                    {
                        ui->label_DevS->setText("配置修改模式");
                        devstage = 2;
                    }
                    else if( array.at(0) == Ymodem_C )
                    {
                        YgetC_flag = 1;
                        ui->label_DevS->setText("固件升级模式");
                        devstage = 3;
                    }
                    else if( array.at(0) == Ymodem_ACK )
                    {
                        YgetACK_flag = 1;
                    }
                }//对14字节的命令帧数据进行接收
                else if( array.size() == 14 )
                {
                    for( int i = 0; i < 14; i++ )
                    {
                        commandRXframe[i] = array.at(i);
                    }
                    if(commandRXframe[0] == 0x01 && commandRXframe[1] == 0xAB && commandRXframe[2] == 0xCD && commandRXframe[3] == 0x05 )
                    {
                        //去掉帧首，对8字节有效数据，进行校验CRC校验
                        if( crcCompute(&commandRXframe[4],8) == (commandRXframe[12]<<8 |commandRXframe[13]) )
                        {
                            local_ip[2] = commandRXframe[6];
                            local_ip[3] = commandRXframe[7];

                            gateway[2] = commandRXframe[8];
                            gateway[3] =  commandRXframe[9];

                            mac[4] = commandRXframe[10];
                            mac[5] = commandRXframe[11];

                            ui->Dev_IP->setText(QString("%1.%2.%3.%4").arg(local_ip[0]).arg(local_ip[1]).arg(local_ip[2]).arg(local_ip[3]));
                            ui->Dev_Gateway->setText(QString("%1.%2.%3.%4").arg(gateway[0]).arg(gateway[1]).arg(gateway[2]).arg(gateway[3]));
                            ui->Dev_Mac->setText(QString("%1.%2.%3.%4.%5.%6").arg(mac[0]).arg(mac[1]).arg(mac[2]).arg(mac[3]).arg(mac[4]).arg(mac[5]));
                            if( commandRXframe[5] == 0x10 && commandRXframe[4] == 0x00)
                            {
                                ui->comboBox->setCurrentIndex(1);

                            }
                            else if( commandRXframe[5] == 0x01 && commandRXframe[4] == 0x00 )
                            {
                                 ui->comboBox->setCurrentIndex(2);
                            }
                        }
                    }

                }

           }
           );


    connect(&timer, &QTimer::timeout,
            [=]()
            {
                if( YgetC_flag == 1 )
                {
//                    ui->Messages_QTE->append("当前有设备进行数据请求");
//                    timer.stop();
                }
                else
                {
//                    ui->Messages_QTE->append("当前无设备进行数据请求");
                }
            }
            );


    connect(&timerPacketSend, &QTimer::timeout,
            [this]()
            {
                //首帧发送阶段
                if( YgetC_flag == 1 && dataSendstage == 1 )
                {
                    startFrame[0] = Ymodem_SOH;
                    startFrame[1] = 0x00;
                    startFrame[2] = 0xFF;

                    //将文件名称以及文件大小数据填充到首帧.START
                    //将QString转换成字符数组char[],以ASCII形式发送;
                    memcpy(startFrame+3, fileName.toLatin1().data(), fileName.size()+1);

                    //文件大小，占据8字节，以quint64形式，保存文件大小XXXXBytes
                    *(quint64*)(&startFrame[3+fileName.size()+1]) = fileSize;
                    qDebug()<<*(quint64*)(&startFrame[3+fileName.size()+1]);

                    startFrame[3+fileName.size()+1+8] = 0x00;

                    for(int i = 0; (3+fileName.size()+1+8+i)<133; i++)
                    {
                        startFrame[3+fileName.size()+1+8+i] = 0x00;
                    }
                    //将文件名称以及文件大小数据填充到首帧.END

                    //计算CRC
                    width_t crc16;
                    crc16 = crcCompute((unsigned char *)startFrame+3,128);
                    qDebug()<<crc16;

                    //写入CRC并发送
                    startFrame[131] = crc16 >> 8;
                    startFrame[132] = crc16;
                    tcpSocket->write(startFrame, 133);

                    YgetC_flag = 0;
                    YgetACK_flag = 0;
                    //进入数据发送阶段
                    dataSendstage++;
                }

                //数据帧阶段
                if(YgetACK_flag == 1 && dataSendstage == 2)
                {
                    static quint16 nowpackNUM = 1;
                    ui->Messages_QTE->append(QString("发送第%1包数据").arg(nowpackNUM));

                    dataFrame[0] = Ymodem_STX;
                    dataFrame[1] = (char)(0x00 + nowpackNUM);
                    dataFrame[2] = (char)(0xFF - nowpackNUM);

                    //判断当前发送的是否是最后一包数据
                    if( nowpackNUM == packetNum )
                    {
                        //写入剩余数据
                        for(int i = 0; i < lastpacketSize; i++)
                        {
                            dataFrame[3+i] = fileAllbuf[(nowpackNUM-1)*1024+i];
                        }
                        //不足1024的部分用0x1A填充
                        for(int i = 0; i < ( 1024-lastpacketSize ); i++)
                        {
                            dataFrame[3+lastpacketSize+i] = 0x1A;
                        }
                        //进入下一阶段
                        dataSendstage ++;
                        nowpackNUM = 0;
                    }
                    //不是最后一包
                    else
                    {
                        for(int i = 0; i < 1024; i++)
                        {
                            dataFrame[3+i] = fileAllbuf[(nowpackNUM-1)*1024+i];
                        }
                    }

                    //计算CRC
                    width_t crc16 = 0;
                    crc16 = crcCompute((unsigned char *)dataFrame+3,1024);
                    //                        qDebug()<<crc16;

                    //写入CRC并发送
                    dataFrame[1027] = crc16 >> 8;
                    dataFrame[1028] = crc16;
                    tcpSocket->write(dataFrame, 1029);

                    YgetACK_flag = 0;
                    nowpackNUM ++;
                }

                //结束帧发送阶段
                if(YgetACK_flag == 1 && dataSendstage == 3)
                {
                    stopFrame[0] = Ymodem_SOH;
                    stopFrame[1] = 0x00;
                    stopFrame[2] = 0xFF;
                    for(int i = 0; i < 128; i++)
                    {
                        stopFrame[3+i] = 0x1A;
                    }
                    //计算CRC
                    width_t crc16 = 0;
                    crc16 = crcCompute((unsigned char *)stopFrame+3,128);
                    //                        qDebug()<<crc16;

                    //写入CRC并发送
                    stopFrame[131] = crc16 >> 8;
                    stopFrame[132] = crc16;
                    tcpSocket->write(stopFrame, 133);

                    dataSendstage = 1;
                    YgetACK_flag = 0;
                    YgetC_flag = 0;
                    timerPacketSend.stop();

                }
            }
           );
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_Connect_PBT_clicked()
{
    if( tcpSocket->state() == QAbstractSocket::UnconnectedState )
    {   //获取服务器ip和端口
        QString ip = ui->IP_IE->text();
        //将Qstring 转换成 int
        quint16 Port = ui->Port_IE->text().toUInt();

        //主动和服务器建立
        tcpSocket->connectToHost(QHostAddress(ip),Port);
    }
    else
    {
        ui->Messages_QTE->append(QString("网络状态:%1").arg(tcpSocket->state()));

    }
}

void Widget::on_Close_PBT_clicked()
{
    if( tcpSocket->state() == QAbstractSocket::ConnectedState )
    {
        //主动和对方断开
        tcpSocket->disconnectFromHost();
        tcpSocket->close();
    }
    else
    {
        ui->Messages_QTE->append(QString("网络状态:%1").arg(tcpSocket->state()));
    }
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

        //只读方式打开文件
        //指定文件的名字
        file.setFileName(filePath);

        //打开文件
        bool isOK = file.open(QIODevice::ReadOnly);
        if(isOK == false)
        {
            qDebug() << "只读方式打开文件失败 333";
        }

        //提示文件打开的路径
        ui->Messages_QTE->append(filePath);
        //关闭文件选择按键，开启发送按键
        ui->Select_PBT->setEnabled(false);
        ui->Send_PBT->setEnabled(true);

        //读取文件原始数据，并且以二位16进制形式打印出来
        QDataStream dataStream(&file);
        memset(fileAllbuf, 0, fileSize);
        dataStream.readRawData(fileAllbuf, fileSize);
        //qDebug()输出存在char[],以16进制的方式
        QString strALL;
        for(quint64 i = 0; i < fileSize; i++)
        {
            strALL += QString::asprintf("%02X ",(uint8_t)fileAllbuf[i]);
        }
        qDebug()<<strALL;

        //打印文件发送信息
        packetNum = fileSize/packetSize + 1;
        lastpacketSize = fileSize%packetSize;
        strALL.clear();
        strALL +="@分包大小:";
        strALL +=QString::asprintf("%d",(qint32)packetSize);
        strALL +="~~~发送文件大小:";
        strALL += QString::asprintf("%d",(qint32)fileSize);
        strALL += "~~~发包数量:";
        strALL += QString::asprintf("%d",(qint16)packetNum);
        strALL += "~~~最后一包的大小:";
        strALL += QString::asprintf("%d",(qint16)lastpacketSize);
        ui->Messages_QTE->append(strALL);

        file.close();
        ui->Messages_QTE->append("文件读取完成");
    }
    else
    {
        qDebug() << "选择文件路径出错 323";
        file.close();
    }


}

void Widget::on_Send_PBT_clicked()
{
    //开启定时器准备好开始发送文件信息
    timerPacketSend.start(100);

}

static unsigned short crcTable[256] =
{
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,

    0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,

    0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,

    0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,

    0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,

    0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,

    0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,

    0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,

    0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,

    0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,

    0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,

    0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,

    0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,

    0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,

    0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,

    0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,

    0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,

    0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,

    0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,

    0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,

    0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,

    0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,

    0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,

    0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,

    0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,

    0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,

    0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,

    0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,

    0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,

    0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,

    0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,

    0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0,
};

/************************************************************************
  * @描述：CRC table 初始化
  * @参数：none
  * @返回值：none
  **********************************************************************/
void crcInit(void)
{
    unsigned short remainder;
    unsigned short dividend;
    int bit;
    /* Perform binary long division, a bit at a time. */
    for(dividend = 0; dividend < 256; dividend++)
    {
        /* Initialize the remainder.  */
        remainder = dividend << (WIDTH - 8);
        /* Shift and XOR with the polynomial.   */
        for(bit = 0; bit < 8; bit++)
        {
            /* Try to divide the current data bit.  */
            if(remainder & TOPBIT)
            {
                remainder = (remainder << 1) ^ POLYNOMIAL;
            }
            else
            {
                remainder = remainder << 1;
            }
        }
        /* Save the result in the table. */
        crcTable[dividend] = remainder;
    }
}

/************************************************************************
  * @描述：CRC 计算
  * @参数：unsigned char * message ：待计算的数据指针
  *        unsigned int nBytes：待计算的数据长度
  * @返回值：CRC值
  **********************************************************************/
width_t crcCompute(unsigned char * message, unsigned int nBytes)
{
    unsigned int offset;
    unsigned char byte;
    width_t remainder = INITIAL_REMAINDER;
    /* Divide the message by the polynomial, a byte at a time. */
    for( offset = 0; offset < nBytes; offset++)
    {
        byte = (remainder >> (WIDTH - 8)) ^ message[offset];
        remainder = crcTable[byte] ^ (remainder << 8);
    }
    /* The final remainder is the CRC result. */
    return (remainder ^ FINAL_XOR_VALUE);
}

void Widget::on_clc1_PBT_clicked()
{
    ui->Messages_QTE->clear();
}

void Widget::on_clc2_PBT_clicked()
{
    ui->revMessage_QTE->clear();
}

//进入设备配置模式
void Widget::on_setting_PBT_clicked()
{

    if( tcpSocket->state() == QAbstractSocket::ConnectedState )
    {
        commandFrame[0] = 0x01;
        commandFrame[1] = 0xAB;
        commandFrame[2] = 0xCD;
        commandFrame[3] = 0x01;
        for( int i = 0; i < 10; i++ )
        {
            commandFrame[i+4] = 0x00;
        }
        tcpSocket->write(commandFrame, 14);
    }
    else
    {

    }
}
//返回上一级
void Widget::on_goBack_PBT_clicked()
{
    if( tcpSocket->state() == QAbstractSocket::ConnectedState )
    {
        commandFrame[0] = 0x01;
        commandFrame[1] = 0xAB;
        commandFrame[2] = 0xCD;
        commandFrame[3] = 0x07;
        for( int i = 0; i < 10; i++ )
        {
            commandFrame[i+4] = 0x00;
        }
        tcpSocket->write(commandFrame, 14);
    }
    else
    {

    }
}

void Widget::on_upgrade_PBT_clicked()
{
    if( tcpSocket->state() == QAbstractSocket::ConnectedState )
    {
        commandFrame[0] = 0x01;
        commandFrame[1] = 0xAB;
        commandFrame[2] = 0xCD;
        commandFrame[3] = 0x02;
        for( int i = 0; i < 10; i++ )
        {
            commandFrame[i+4] = 0x00;
        }
        tcpSocket->write(commandFrame, 14);
    }
    else
    {

    }
}

void Widget::on_readDev_PBT_clicked()
{
    if( tcpSocket->state() == QAbstractSocket::ConnectedState )
    {
        commandFrame[0] = 0x01;
        commandFrame[1] = 0xAB;
        commandFrame[2] = 0xCD;
        commandFrame[3] = 0x06;
        for( int i = 0; i < 10; i++ )
        {
            commandFrame[i+4] = 0x00;
        }
        tcpSocket->write(commandFrame, 14);
    }
    else
    {

    }
}

void Widget::on_changeDev_PBT_clicked()
{
    if( tcpSocket->state() == QAbstractSocket::ConnectedState )
    {
        commandFrame[0] = 0x01;
        commandFrame[1] = 0xAB;
        commandFrame[2] = 0xCD;
        commandFrame[3] = 0x05;
        QString str;
        str = ui->Dev_IP->text();

        local_ip[2] = str.section(".",2,2).toUInt();
        local_ip[3] = str.section(".",3,3).toUInt();

        str.clear();
        str = ui->Dev_Gateway->text();
        gateway[2] = str.section(".",2,2).toUInt();
        gateway[3] = str.section(".",3,3).toUInt();

        str.clear();
        str = ui->Dev_Mac->text();
        mac[4] = str.section(".",4,4).toUInt();
        mac[5] = str.section(".",5,5).toUInt();

        qDebug() << QString("%1.%2.%3.%4").arg(local_ip[0]).arg(local_ip[1]).arg(local_ip[2]).arg(local_ip[3]);
        qDebug() << QString("%1.%2.%3.%4").arg(gateway[0]).arg(gateway[1]).arg(gateway[2]).arg(gateway[3]);
        qDebug() << QString("%1.%2.%3.%4.%5.%6").arg(mac[0]).arg(mac[1]).arg(mac[2]).arg(mac[3]).arg(mac[4]).arg(mac[5]);

        if( ui->comboBox->currentIndex() == 1 )
        {
            commandFrame[4] = 0x00;
            commandFrame[5] = 0x10;

            commandFrame[6] = local_ip[2];
            commandFrame[7] = local_ip[3];

            commandFrame[8] = gateway[2];
            commandFrame[9] = gateway[3];

            commandFrame[10] = mac[4];
            commandFrame[11] = mac[5];

            width_t crc16 = 0;
            crc16 = crcCompute((unsigned char *)commandFrame+4,8);

            commandFrame[12] = crc16 >> 8;
            commandFrame[13] = crc16;
            tcpSocket->write(commandFrame, 14);
        }
        else if( ui->comboBox->currentIndex() == 2 )
        {
            commandFrame[4] = 0x00;
            commandFrame[5] = 0x01;

            commandFrame[6] = local_ip[2];
            commandFrame[7] = local_ip[3];

            commandFrame[8] = gateway[2];
            commandFrame[9] = gateway[3];

            commandFrame[10] = mac[4];
            commandFrame[11] = mac[5];

            width_t crc16 = 0;
            crc16 = crcCompute((unsigned char *)commandFrame+4,8);

            commandFrame[12] = crc16 >> 8;
            commandFrame[13] = crc16;
            tcpSocket->write(commandFrame, 14);
        }

//        commandFrame[6] = local_ip[2];
//        commandFrame[7] = local_ip[3];

//        commandFrame[8] = gateway[2];
//        commandFrame[9] = gateway[3];

//        commandFrame[10] = mac[4];
//        commandFrame[11] = mac[5];

//        width_t crc16 = 0;
//        crc16 = crcCompute((unsigned char *)commandFrame+4,8);

//        commandFrame[12] = crc16 >> 8;
//        commandFrame[13] = crc16;

//        qDebug() << QString::asprintf("0x%02X.0x%02X.0x%02X.0x%02X.0x%02X.0x%02X.%d.%d.%d.%d.%d.%d.0x%02X.0x%02X",
//                (quint8)commandFrame[0],(quint8)commandFrame[1],(quint8)commandFrame[2],
//                (quint8)commandFrame[3],(quint8)commandFrame[4],(quint8)commandFrame[5],
//                (quint8)commandFrame[6],(quint8)commandFrame[7],(quint8)commandFrame[8],
//                (quint8)commandFrame[9],(quint8)commandFrame[10],(quint8)commandFrame[11],
//                (quint8)commandFrame[12],(quint8)commandFrame[13]                           );
    }

}

void Widget::on_goto_PBT_clicked()
{
    if( tcpSocket->state() == QAbstractSocket::ConnectedState )
    {
        commandFrame[0] = 0x01;
        commandFrame[1] = 0xAB;
        commandFrame[2] = 0xCD;
        commandFrame[3] = 0x03;
        for( int i = 0; i < 10; i++ )
        {
            commandFrame[i+4] = 0x00;
        }
        tcpSocket->write(commandFrame, 14);
    }
    else
    {

    }
}
