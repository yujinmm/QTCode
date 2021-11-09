#ifndef CLIENTWIDGET_H
#define CLIENTWIDGET_H

#include <QWidget>
#include <QTcpSocket>    //通信套接字

namespace Ui {
class ClientWidget;
}

class ClientWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ClientWidget(QWidget *parent = nullptr);
    ~ClientWidget();

private slots:
    void on_pushButton_Connect_clicked();

    void on_pushButton_Send_clicked();

    void on_pushButton_Close_clicked();

private:
    Ui::ClientWidget *ui;

    QTcpSocket *tcpSocket;   //通信
};

#endif // CLIENTWIDGET_H
