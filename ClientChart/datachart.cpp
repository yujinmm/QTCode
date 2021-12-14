#include "datachart.h"
#include "ui_datachart.h"

dataChart::dataChart(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::dataChart)
{
    ui->setupUi(this);

    paper = new QChartView(this);
   //setchart 是否会将pen视为paper的儿子
    pen = new QChart();
    paper->setChart(pen);

    pen->setTitle("传感器原始数据");
    pen->setTheme(QChart::ChartThemeDark);
    pen->layout()->setContentsMargins(0,0,0,0);

    //XY方向同时放大到鼠标画出的矩形大小（也可以设置为只放大X轴或Y轴）,右键缩小???
    paper->setRubberBand(QChartView::RectangleRubberBand);

    //新建坐标轴
    zhouX = new QValueAxis;
    zhouY = new QValueAxis;
    zhouX->setRange(0,Xcount);
    zhouX->setTitleText("采样点");
   // Dialog *dlg = new Dialog
   // connect(widget,SIGNAL(sendY_min(QString)),this,SLOT(receiveY_min(QString)));

    zhouY->setRange(1000,2000);

   // qDebug ()<< _Y_max;
   // qDebug ()<< _Y_min;
    zhouY->setTitleText("数字量");

    series = new QLineSeries;  //序列点
    series->setUseOpenGL();
    series->setName("序列1");  //图例名称
    pen->addSeries(series);


    //显示坐标轴
//    pen->addAxis(zhouX,Qt::AlignBottom);
//    pen->addAxis(zhouY,Qt::AlignLeft);
    pen->setAxisX(zhouX,series);
    pen->setAxisY(zhouY,series);

    //添加布局
    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->setContentsMargins(0, 0, 0, 0);//布局填满窗口
    vLayout->addWidget(paper);
    this->setLayout(vLayout);
}

dataChart::~dataChart()
{
    delete ui;
}
