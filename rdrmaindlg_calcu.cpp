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

void rdrMainDlg::on_pStart_clicked()
{
//*************************************SELECT DATA*****************************************
//clear containers
    ui->pTable->setRowCount(0);
    ui->pTable->clearContents();
    int rows = ui->pTable->rowCount();
    data._valuesCa.clear();
    data._valuesMEA.clear();
    data.transList.clear();
    QVector <float> _regionIntensity;           //mean intensity in the ROI

if(_electrodeR.pRegionPathItem != nullptr)
{
    _regionIntensity.clear();
    pRgSeries = new QLineSeries();
    pRgSeries->setName("electrode");
    pRgSeries->setPen(QPen(Qt::red));

    QRectF _boundingRect = _electrodeR.pRegionPathItem->boundingRect();
    double _sum=0;
    double _count=0;
    long w=data._frameLst.at(0)->width();
    QPointF pnt;
/*
    QImage I(*(data._frameLst.at(0)));
    QPainter pain(&I);
    pain.setPen(QPen(Qt::red));
*/
    for (long t=0;t<data._framesInFloat.count();++t)
    {
        _sum=_count=0.0;
        for (long x=_boundingRect.left();x<_boundingRect.right();++x)
            for (long y=_boundingRect.top();y<_boundingRect.bottom();++y)
                if (_electrodeR.pRegionPathItem->path().contains(QPointF(x,y)))
                {
                    pnt = img->mapFromScene(_electrodeR.pRegionPathItem->mapToScene(QPointF(x,y)));
                    _sum+=qGray(data._frameLst.at(t)->pixel(pnt.x(),pnt.y()));
                    //_sum+=data._framesInFloat.at(t)[(int)pnt.x()+(int)pnt.y()*w];
                    _count++;

                   //pain.drawPoint(pnt);

                }
        //pain.end();
        //I.save("/home/xpit/tmp/test.png");

        emit message(QString("%1 %2 %3").arg(_sum).arg(_count).arg(_sum/_count));
        _count>0 ? _regionIntensity.append(_sum/_count) : _regionIntensity.append(0);
        pRgSeries->append(t,_regionIntensity.last());
    }

    ui->pProgressBar->setValue(20);
    ui->pProgressLbl->setText("Fox runs over peaks and valleys");
    //find minimum peaks
    findMinPositions(_regionIntensity,data._minPos,0, _regionIntensity.count()-1);

    //find maximum peaks
    findMaxPositions(_regionIntensity,data._maxPos,0,_regionIntensity.count()-1);
    if (data._minPos.count()==0 || data._maxPos.count()==0){ emit message ("Minimums and maximums were not calculated"); return;}

    //find absolute min and max
    float absMin = _regionIntensity.at(data._minPos.at(0));
    float absMax = _regionIntensity.at(data._maxPos.at(0));
    for(int i = 0; i<data._minPos.size(); i++)
    {
        absMin= min(_regionIntensity.at(data._minPos.at(i)), absMin);
    }
    for(int i = 0; i<data._maxPos.size(); i++)
    {
        absMax= max(_regionIntensity.at(data._maxPos.at(i)), absMax);
    }
    for(int j = 0; j<_regionIntensity.size(); j++)
    {
        _regionIntensity[j] = (_regionIntensity.at(j)-absMin)/(absMax-absMin);
    }

    // create a 2D series for calcium
    QLineSeries *pCal=new QLineSeries();
    QPointF point;
    for (int i=0;i<_regionIntensity.count();++i)
    {
        point.setX((float)i/data._frameRate*1000.0f);
        point.setY(_regionIntensity.at(i));
        pCal->append(QPointF((float)i/data._frameRate*1000.0f,_regionIntensity.at(i)));
        data._valuesCa.append(point);
    }

    //create individual transients
    data.transList.clear();
    for(int i = 0; i<data._minPos.count()-1; i++)
    {
        ui->pErrorTxtB->append(QString("%1").arg(i));
        caTransient *pcaT = new caTransient();
        //connect(pcaT,SIGNAL(message(const QString&)),this,SLOT(printMessage(const QString&)));
        pcaT->calculateValues(data, data._minPos.at(i), data._minPos.at(i+1), data._maxPos.at(i));
        data.transList.append(pcaT);
    }

//**************************************PLOT SERIES*****************************************
//plot the minimums and maximums
    QScatterSeries *pminSSeries = new QScatterSeries();
    for (int i=0;i<data._minPos.count();++i)
        pminSSeries->append(QPointF((float)data._minPos.at(i)/data._frameRate*1000.0f,_regionIntensity.at(data._minPos.at(i))));
    QScatterSeries *pmaxSSeries = new QScatterSeries();
    for (int i=0;i<data._maxPos.count();++i)
        pmaxSSeries->append(QPointF((float)data._maxPos.at(i)/data._frameRate*1000.0f,_regionIntensity.at(data._maxPos.at(i))));


//plot electrode data
    QLineSeries *pMEA = new QLineSeries();
    if(!data._electrodeMap.isEmpty())
    {
        float startPnt = data._strtPnt.at(ui->pStartingPoint->currentIndex());      //µs
        //float nextStart = data._strtPnt.at(ui->pStartingPoint->currentIndex()+1);
        bool _found=false;
        long _index=-1;
        while (!_found && _index<data._electrodeMap["TimeStamp [µs]"].size())
            _found = startPnt<=data._electrodeMap["TimeStamp [µs]"].at(++_index);

        if (_found)
        {
            QString keyElectrode= ui->pElectrode->currentText();
            float _minElectrode = data._electrodeMap[keyElectrode].at(_index);
            float _maxElectrode = data._electrodeMap[keyElectrode].at(_index);

            float _endTime = data._valuesCa.last().rx()*1000.0f;

            for (long long e =_index; e<data._electrodeMap[keyElectrode].size() && _endTime>data._electrodeMap[keyElectrode].at(e); e++)
            {
                _minElectrode = min(data._electrodeMap[keyElectrode].at(e), _minElectrode);
                _maxElectrode = max(data._electrodeMap[keyElectrode].at(e), _maxElectrode);
            }

            data._valuesMEA.clear();
            for (long long e =_index; e<data._electrodeMap[keyElectrode].size() && _endTime>data._electrodeMap[keyElectrode].at(e); e++)
            {
                point.setX((data._electrodeMap["TimeStamp [µs]"].at(e)-startPnt)/1000.0f);
                point.setY(data._electrodeMap[keyElectrode].at(e));
                pMEA->append(point.x(), (point.y()-_minElectrode)/(_maxElectrode-_minElectrode));
                data._valuesMEA.append(point);
            }

        }
    }


    pCal->setColor(Qt::red);
    pMEA->setColor(Qt::white);
    pminSSeries->setColor(Qt::green);
    pmaxSSeries->setColor(Qt::darkGreen);

//plot transient intermediate points
    QScatterSeries *transPoints = new QScatterSeries();
    for(int i=0; i<data.transList.count(); i++)
    {
        if(data.transList.at(i)->lmin20!=-1)
        transPoints->append(data._valuesCa.at(data.transList.at(i)->lmin20).x(), data._valuesCa.at(data.transList.at(i)->lmin20).y());
        if(data.transList.at(i)->lmid50!=-1)
         transPoints->append(data._valuesCa.at(data.transList.at(i)->lmid50).x(), data._valuesCa.at(data.transList.at(i)->lmid50).y());
        if(data.transList.at(i)->lmax90!=-1)
          transPoints->append(data._valuesCa.at(data.transList.at(i)->lmax90).x(), data._valuesCa.at(data.transList.at(i)->lmax90).y());
        if(data.transList.at(i)->rmax90!=-1)
           transPoints->append(data._valuesCa.at(data.transList.at(i)->rmin20).x(), data._valuesCa.at(data.transList.at(i)->rmin20).y());
        if(data.transList.at(i)->rmid50!=-1)
            transPoints->append(data._valuesCa.at(data.transList.at(i)->rmid50).x(), data._valuesCa.at(data.transList.at(i)->rmid50).y());
        if(data.transList.at(i)->rmax90!=-1)
             transPoints->append(data._valuesCa.at(data.transList.at(i)->rmax90).x(), data._valuesCa.at(data.transList.at(i)->rmax90).y());
    }
    QChart *pChart = new QChart();
    pChart->setTheme(QChart::ChartThemeDark);
    pChart->addSeries(pminSSeries); //add minimum peaks
    pChart->addSeries(pmaxSSeries); //add maximum peaks
    pChart->addSeries(pCal); //add bakcground corrected curve
    pChart->addSeries(pMEA); //add MEA
    pChart->addSeries(transPoints); // add transient checkpoints)

//format chart
    pChart->createDefaultAxes();
    pChart->legend()->setVisible(false);

    pChartView->setChart(pChart);
    QList<QAbstractAxis *> _axesLst = pChart->axes(Qt::Horizontal);
    QList<QAbstractAxis *> _axisY =pChart->axes(Qt::Vertical);
    if (_axesLst.count()>0)
    {
        _axesLst.at(0)->setMax(data._frameLst.count()/data._frameRate*1000.0f);
        _axesLst.at(0)->setMin(0);
    }
    if(_axisY.count()>0)
    {
        _axisY.at(0)->setMax(1.1f);
        _axisY.at(0)->setMin(-0.05);
    }

//************************************BEGIN CALCULATIONS AND POPULATE THE TABLE*************************************
// get the peak frequency parameter
    float x2=0.0f ,x1=0.0f;
    QList <float> cValues;
    for(int i=0;i<data.transList.count()-1; i++)
    {
        x2=data._valuesCa.at((data.transList.at(i+1)->_maxabs)).x();
        x1=data._valuesCa.at((data.transList.at(i)->_maxabs)).x();
        cValues.append(x2-x1);
        data.peakFreq= data.peakFreq+(1.0/(cValues.at(i)));
    }
    data.peakFreq/=data.transList.count()-1;
    data.peakFreq*=1000.0;//convert into seconds

//get the irregularity
    for(int j=0; j<data.transList.count()-1; j++)
    {
        data.peakIrregularity += pow(data.peakFreq - 1.0/(cValues.at(j)/1000.0) ,2);
    }
    data.peakIrregularity/=data.transList.count()-2;
    data.peakIrregularity = sqrt(data.peakIrregularity);

//populate table
    for(int transNr = 0; transNr<data.transList.count(); transNr++)
    {


        fillTable(QString("%1").arg(transNr),
                  data.transList.at(transNr)->transTime,
                  data.transList.at(transNr)->timeToPeak,
                  data.transList.at(transNr)->rise,
                  data.transList.at(transNr)->riseR2,
                  data.transList.at(transNr)->decay,
                  data.transList.at(transNr)->decayR2,
                  data.transList.at(transNr)->ascSlope,
                  data.transList.at(transNr)->descSlope,
                  data.transList.at(transNr)->patHalfMax,
                  data.transList.at(transNr)->riseTime,
                  data.transList.at(transNr)->decayTime);

    }

    mean(&data);
//************************************************************************************************************
    }
}
