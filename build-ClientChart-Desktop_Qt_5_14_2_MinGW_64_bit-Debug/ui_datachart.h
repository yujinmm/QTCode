/********************************************************************************
** Form generated from reading UI file 'datachart.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DATACHART_H
#define UI_DATACHART_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_dataChart
{
public:

    void setupUi(QWidget *dataChart)
    {
        if (dataChart->objectName().isEmpty())
            dataChart->setObjectName(QString::fromUtf8("dataChart"));
        dataChart->resize(695, 440);

        retranslateUi(dataChart);

        QMetaObject::connectSlotsByName(dataChart);
    } // setupUi

    void retranslateUi(QWidget *dataChart)
    {
        dataChart->setWindowTitle(QCoreApplication::translate("dataChart", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class dataChart: public Ui_dataChart {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DATACHART_H
