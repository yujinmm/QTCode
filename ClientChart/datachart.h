#ifndef DATACHART_H
#define DATACHART_H

#include <QWidget>
#include <QtCharts>
#include <QVector>

#define Xcount 4000

namespace Ui {
class dataChart;
}

class dataChart : public QWidget
{
    Q_OBJECT

public:
    explicit dataChart(QWidget *parent = nullptr);
    ~dataChart();

private:
    Ui::dataChart *ui;

    QVector<quint16> *vdata;
    QLineSeries *series;
    QValueAxis *zhouX,*zhouY;

    QChartView *paper;
    QChart *pen;
};

#endif // DATACHART_H
