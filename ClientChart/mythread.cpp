#include "mythread.h"

myThread::myThread(QObject *parent) : QObject(parent)
{
        qRegisterMetaType<QVector<QPointF>>("QList<diskInformation>");
}

myThread::myThread(int ch , int xnum)
{
    qRegisterMetaType<QVector<QPointF>>("QList<diskInformation>");
    xcount = xnum;
    vpd1.resize(xcount);
    vsd1.resize(xcount);
}

void myThread::dealData(int i , QByteArray byteArray)
{
    if(i==1)
    {
        for(int i = 0, j = 0; i < 200; i++, j = j+2)
        {

            vsd1.push_back( (int16_t )(((byteArray.at( 0+j )<<8)&0xff00) | (byteArray.at( 1+j ) & 0x00ff)) + 1000);
            //        vsd.push_back(((byteArray.at( channelSplit1+j )<<8)&0xff00) | (byteArray.at( channelSplit2+j ) & 0x00ff));
            //        vpd.push_back(QPointF(i,((byteArray.at( 6+j )<<8)&0xff00) | (byteArray.at( 7+j ) & 0x00ff)));
            //        vpd.push_back(QPointF(0,((byteArray.at( 0+j )<<8)&0xff00) | (byteArray.at( 1+j ) & 0x00ff)));
            if( vsd1.size() > xcount )
            {
                vsd1.remove(0,200);
                //            vpd.erase(vpd.begin());
            }
        }
        for(int i = 0; i < xcount; i++)
        {
            vpd1[i]=QPointF(i,vsd1.at(i));
        }
        //    lo.unlock();
        emit startprint(1,vpd1);
    }
}
