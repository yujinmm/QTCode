#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QObject>
#include <QVector>
#include <QPointF>

class myThread : public QObject
{
    Q_OBJECT
public:
    explicit myThread(QObject *parent = nullptr);

    myThread(int ch , int j );

    //线程处理函数
    void dealData(int i , QByteArray byteArray);

signals:

        void startprint(int i , QVector<QPointF> v);
public:
        QVector<QPointF>  vpd1;
        QVector<quint16>  vsd1;
        int xcount;
};

#endif // MYTHREAD_H
