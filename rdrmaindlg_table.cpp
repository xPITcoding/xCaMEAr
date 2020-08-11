#include "rdrmaindlg.h"
#include "ui_rdrmaindlg.h"
#include "effecttoolbutton.h"
#include "iostream"
#include "rdr_calc.h"
#include "xrsettingswdgt.h"
#include "catransient.h"

#include <QMessageBox>
#include <QDir>
#include <QFileDialog>
#include <QTextStream>
#include <QTimer>
#include <QDate>
#include <QGraphicsSceneMouseEvent>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QScatterSeries>
#include <QCategoryAxis>
#include <QMovie>
#include <QTimer>

#include "tiffio.h"
#include "math.h"
using namespace std;

void rdrMainDlg::fillTable(QString peak, float transTime, float timeToPeak, QVector<float> rise, float riseR2, QVector <float> decay, float decayR2, float ascSlope, float descSlope, float timeathalfmax, float risetime, float decaytime, float rt50)
{
    QTableWidgetItem* pPeak = new QTableWidgetItem;
    if(peak != -1)
    {
        pPeak->setText(peak);
        pPeak->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    }
    else{
        pPeak->setText("n/a");
    }

    QTableWidgetItem* pTransT=  new QTableWidgetItem;
    if(transTime != -1)
    {
        pTransT->setText(QString("%1").arg(transTime, 0,'f',3));
        pTransT->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    }
    else{
        pTransT->setText("n/a");
    }

    QTableWidgetItem* ptoPeakT=  new QTableWidgetItem;
    if(timeToPeak != -1)
    {
        ptoPeakT->setText(QString("%1").arg(timeToPeak, 0,'f',3));
        ptoPeakT->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    }
    else{
        ptoPeakT->setText("n/a");
    }

    QTableWidgetItem* pRise =  new QTableWidgetItem;
    if(!rise.isEmpty()){
        pRise->setText(QString("%1").arg(rise[0], 0,'f',3)+";"+QString("%1").arg(rise[1], 0,'f',3)+ ";"+ QString("%1").arg(rise[2], 0,'f',3));
        pRise->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);}
    else{
        pRise->setText("n/a");
    }
    QTableWidgetItem* pRiseR2 =  new QTableWidgetItem;
    if(riseR2 != -1)
    {
        pRiseR2->setText(QString("%1").arg(riseR2, 0,'f',3));
        pRiseR2->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    }
    else{
        pRiseR2->setText("n/a");
    }
    QTableWidgetItem* pDecay=  new QTableWidgetItem;
    if(!decay.isEmpty()){
        pDecay->setText(QString("%1").arg(decay[0], 0,'g',3));
        pDecay->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    }
    else{
        pDecay->setText("n/a");
    }

    QTableWidgetItem* pDecayR2=  new QTableWidgetItem;
    if(decayR2 != -1)
    {
        pDecayR2->setText(QString("%1").arg(decayR2,0,'f',3));
        pDecayR2->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    }
    else{
        pDecayR2->setText("n/a");
    }

    QTableWidgetItem* pAscSlope=  new QTableWidgetItem;
    if(ascSlope != -1)
    {
        pAscSlope->setText(QString("%1").arg(ascSlope,0,'f',3));
        pAscSlope->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    }
    else{
        pAscSlope->setText("n/a");
    }

    QTableWidgetItem* pDescSlope=  new QTableWidgetItem;
    if(descSlope != -1)
    {
        pDescSlope->setText(QString("%1").arg(descSlope,0,'f',3));
        pDescSlope->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    }
    else{
        pDescSlope->setText("n/a");
    }

    QTableWidgetItem* pTimeAtHalfMax=  new QTableWidgetItem;
    if(timeathalfmax != -1)
    {
        pTimeAtHalfMax->setText(QString("%1").arg(timeathalfmax,0,'f',3));
        pTimeAtHalfMax->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    }
    else{
        pTimeAtHalfMax->setText("n/a");
    }

    QTableWidgetItem* pRiseTime=  new QTableWidgetItem;
    if(risetime != -1)
    {
        pRiseTime->setText(QString("%1").arg(risetime,0,'f',3));
        pRiseTime->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    }
    else{
        pRiseTime->setText("n/a");
    }

    QTableWidgetItem* pDecayTime=  new QTableWidgetItem;
    if(decaytime != -1)
    {
        pDecayTime->setText(QString("%1").arg(decaytime,0,'f',3));
        pDecayTime->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    }
    else{
        pDecayTime->setText("n/a");
    }

    QTableWidgetItem* pRT50=  new QTableWidgetItem;
    if(decaytime != -1)
    {
        pRT50->setText(QString("%1").arg(rt50,0,'f',3));
        pRT50->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    }
    else{
        pRT50->setText("n/a");
    }


    ui->pTable->setRowCount(ui->pTable->rowCount()+1);
    int row = ui->pTable->rowCount()-1;
    ui->pTable->setItem(row,0,pPeak);
    ui->pTable->setItem(row,1,pTransT);
    ui->pTable->setItem(row,2,ptoPeakT);
    ui->pTable->setItem(row,3,pRise);
    ui->pTable->setItem(row,4,pRiseR2);
    ui->pTable->setItem(row,5,pDecay);
    ui->pTable->setItem(row,6,pDecayR2);
    ui->pTable->setItem(row,7,pAscSlope);
    ui->pTable->setItem(row,8,pDescSlope);
    ui->pTable->setItem(row,9,pTimeAtHalfMax);
    ui->pTable->setItem(row,10,pRiseTime);
     ui->pTable->setItem(row,11,pDecayTime);
     ui->pTable->setItem(row,12,pRT50);


    for (int c=0;c<ui->pTable->columnCount();++c) ui->pTable->resizeColumnToContents(c);
}

