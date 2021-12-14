/********************************************************************************
** Form generated from reading UI file 'tcpclient.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TCPCLIENT_H
#define UI_TCPCLIENT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TcpClient
{
public:
    QPushButton *clear_PB;
    QWidget *widget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLineEdit *IP_lineEdit;
    QPushButton *connect_PB;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *Port_lineEdit;
    QPushButton *disconnect_PB;
    QTextEdit *Messages_QTE;

    void setupUi(QWidget *TcpClient)
    {
        if (TcpClient->objectName().isEmpty())
            TcpClient->setObjectName(QString::fromUtf8("TcpClient"));
        TcpClient->resize(310, 270);
        clear_PB = new QPushButton(TcpClient);
        clear_PB->setObjectName(QString::fromUtf8("clear_PB"));
        clear_PB->setGeometry(QRect(270, 230, 31, 31));
        QFont font;
        font.setFamily(QString::fromUtf8("\345\215\216\345\205\211\350\241\214\344\271\246_CNKI"));
        clear_PB->setFont(font);
        widget = new QWidget(TcpClient);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setGeometry(QRect(10, 10, 258, 256));
        verticalLayout = new QVBoxLayout(widget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        IP_lineEdit = new QLineEdit(widget);
        IP_lineEdit->setObjectName(QString::fromUtf8("IP_lineEdit"));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\345\215\216\345\205\211\350\241\214\344\271\246_CNKI"));
        font1.setPointSize(12);
        IP_lineEdit->setFont(font1);

        horizontalLayout->addWidget(IP_lineEdit);

        connect_PB = new QPushButton(widget);
        connect_PB->setObjectName(QString::fromUtf8("connect_PB"));
        connect_PB->setFont(font1);

        horizontalLayout->addWidget(connect_PB);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        Port_lineEdit = new QLineEdit(widget);
        Port_lineEdit->setObjectName(QString::fromUtf8("Port_lineEdit"));
        Port_lineEdit->setFont(font1);

        horizontalLayout_2->addWidget(Port_lineEdit);

        disconnect_PB = new QPushButton(widget);
        disconnect_PB->setObjectName(QString::fromUtf8("disconnect_PB"));
        disconnect_PB->setFont(font1);

        horizontalLayout_2->addWidget(disconnect_PB);


        verticalLayout->addLayout(horizontalLayout_2);

        Messages_QTE = new QTextEdit(widget);
        Messages_QTE->setObjectName(QString::fromUtf8("Messages_QTE"));
        QFont font2;
        font2.setFamily(QString::fromUtf8("\345\215\216\345\205\211\350\241\214\344\271\246_CNKI"));
        font2.setPointSize(10);
        Messages_QTE->setFont(font2);

        verticalLayout->addWidget(Messages_QTE);


        retranslateUi(TcpClient);

        QMetaObject::connectSlotsByName(TcpClient);
    } // setupUi

    void retranslateUi(QWidget *TcpClient)
    {
        TcpClient->setWindowTitle(QCoreApplication::translate("TcpClient", "Form", nullptr));
        clear_PB->setText(QCoreApplication::translate("TcpClient", "CLC", nullptr));
        IP_lineEdit->setText(QCoreApplication::translate("TcpClient", "192.168.0.119", nullptr));
        connect_PB->setText(QCoreApplication::translate("TcpClient", "\350\277\236\346\216\245", nullptr));
        Port_lineEdit->setText(QCoreApplication::translate("TcpClient", "12927", nullptr));
        disconnect_PB->setText(QCoreApplication::translate("TcpClient", "\346\226\255\345\274\200", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TcpClient: public Ui_TcpClient {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TCPCLIENT_H
