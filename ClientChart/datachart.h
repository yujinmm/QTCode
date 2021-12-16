#ifndef DATACHART_H
#define DATACHART_H

#include <QWidget>
#include <QtCharts>
#include <QVector>

#define Xcount 2000

namespace Ui {
class dataChart;
}

class dataChart : public QWidget
{
    Q_OBJECT

public:
    explicit dataChart(QWidget *parent = nullptr);
    ~dataChart();

    QLineSeries *series;

private:
    Ui::dataChart *ui;

    QVector<quint16> *vdata;

    QValueAxis *zhouX,*zhouY;

    QChartView *paper;
    QChart *pen;
};

#endif // DATACHART_H
