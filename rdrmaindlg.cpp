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


rdrMainDlg::rdrMainDlg(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::rdrMainDlg)
{
    ui->setupUi(this);
    ui->pProgressBar->setMinimum(2);
    ui->pProgressBar->setMaximum(10);
    QGraphicsScene* pDisplayG= new QGraphicsScene;
    ui->pDisplay->setScene(pDisplayG);
    pMovieTimer = new QTimer(this);
    pMovieTimer->setSingleShot(false);
    connect(pMovieTimer,SIGNAL(timeout()), this, SLOT(play_next_Frame()));
    _sceneActive = false;

    pChartView = new QChartView(this);
    QChart* pEmptyChart = new QChart();
    pEmptyChart->setTheme(QChart::ChartThemeDark);
    pChartView->setGeometry(610,11,591,341);
    pChartView->setPalette(palette());
    pChartView->setAutoFillBackground(true);
    pChartView->setChart(pEmptyChart);
    pChartView->setRubberBand(QChartView::RectangleRubberBand);

    pSettingsWdgt = new xrSettingsWdgt(this);
    pSettingsWdgt->hide();

    connect(this,SIGNAL(message(const QString&)),this,SLOT(printMessage(const QString &)));

    ui->tabWidget->setCurrentIndex(1);

    QFile f( ::_settings["last imported file"]._stringValue);
    if (f.open(QFile::ReadOnly))
    {
        QTextStream t(&f);
        ui->pCalciumLE->setText(t.readLine());
        ui->pBackgroundLE->setText(t.readLine());
        ui->pElectrodesLE->setText(t.readLine());
        ui->pDigitalLE->setText(t.readLine());
        f.close();
    }

    QMovie* ice = new QMovie(":/images/yet_another_running_fox.gif");
    ice->setScaledSize(ui->icelabel->size());
    ui->icelabel->setMovie(ice);
    ice->start();
}

rdrMainDlg::~rdrMainDlg()
{
    delete ui;
}
void rdrMainDlg::printMessage(const QString& txt)
{
    ui->pErrorTxtB->append(txt);
}
void rdrMainDlg::play_next_Frame()
{
    ui->pSlider->setValue((ui->pSlider->value()+1)% ui->pSlider->maximum());
}
void rdrMainDlg::on_pQuit_clicked()
{
    QMessageBox warn(0);
    warn.setText("Are you sure you want to quit?");
    warn.setInformativeText("All unexported data will be lost.");
    warn.setWindowTitle("Warning");
    warn.setStandardButtons(QMessageBox::Save | QMessageBox::Ok | QMessageBox::Cancel);
    int ret = warn.exec();
    switch (ret)
    {
    case QMessageBox::Save:
        ui->pExport->click();
        break;
    case QMessageBox::Ok:
        accept();
        break;
    case QMessageBox::Cancel:
        warn.close();
        break;
    }

}

void rdrMainDlg::updateStack(int value)
{

    float _timeelps = (float)value/data._frameRate;
    ui->timelbl->setText(QString("%1").arg(_timeelps, 0,'f',3));
    displaySlice(value);
}

void rdrMainDlg::createRegion(QPointF pos)
{
  createStandardPathItems();
}

void rdrMainDlg::zoom(int pos)
{
    pos>0 ? ui->pDisplay->scale(1.1,1.1) : ui->pDisplay->scale(0.9,0.9);
}

void rdrMainDlg::displaySlice(int i)
{
    /*
    long w = (data._frameLst.at(i))->width();
    long h = (data._frameLst.at(i))->height();
    int v;
    QImage I(w,h,QImage::Format_Grayscale8);
    for (long x=0;x<w;++x)
        for (long y=0;y<h;++y)
        {
            v = ((float)qGray((data._frameLst.at(i))->pixel(x,y))-data._minVal)/(data._maxVal-data._minVal);
            I.setPixelColor(x,y,QColor(v,v,v));
        }
    */
    QPixmap pix;
    bool b=pix.convertFromImage(*(data._frameLst.at(i)));

    if(img==nullptr)
    {
        img = new rdrCustomPixmapItem();
        ui->pDisplay->scene()->addItem(img);
        connect(img,SIGNAL(createRegion(QPointF)),this,SLOT(createRegion(QPointF)));
        connect(img,SIGNAL(zoom(int)),this,SLOT(zoom(int)));
    }
    img->setPixmap(pix);
    img->setVisible(true);
    img->setZValue(100);
    img->setPos(0,0);
    ui->pDisplay->scene()->update();

    if(!img->pixmap().isNull())
    {
        float scale = std::max((float)img->pixmap().width() / (float)ui->pDisplay->width(),(float)img->pixmap().height() / (float)ui->pDisplay->height());
                img->setScale(1.0f/scale);
    }
}

void rdrMainDlg::on_pSlider_valueChanged(int value)
{
    if(_sceneActive==true)
    updateStack(value);
    else
    {

    }
}


void rdrMainDlg::on_pPlay_clicked()
{
    pMovieTimer->setInterval(1.0f/data._frameRate*1000.0f);
    pMovieTimer->start();
}

void rdrMainDlg::on_pPause_clicked()
{
    pMovieTimer->stop();
}

void rdrMainDlg::electrodeHandleItemMoved(int id)
{
    _electrodeR.updatePath(_electrodeR.pRegionPathItem->regionType());
}

void rdrMainDlg::createStandardPathItems()
{

    if (_electrodeR.pRegionPathItem) delete _backgrounR.pRegionPathItem;_backgrounR.pRegionPathItem=NULL;

       if (!_electrodeR.pRegionPathItem)
    {
        _electrodeR.pRegionPathItem = new rdrPathItem(img);
        _electrodeR.pRegionPathItem->setZValue(1);
        _electrodeR.pRegionPathItem->setFlags(QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsSelectable);
        _electrodeR.pRegionGroupItem = new QGraphicsItemGroup(0);
        _electrodeR.pRegionGroupItem->addToGroup(_backgrounR.pRegionPathItem);
        _electrodeR.pRegionGroupItem->setFlags(QGraphicsItem::ItemIsMovable);

        _electrodeR._nodes.clear();
        _electrodeR._nodes.append(rdrRegionControlNode(0,QPoint(25,0),XLFLLCNT_POINT));
        _electrodeR._nodes.append(rdrRegionControlNode(1,QPoint(50,25),XLFLLCNT_POINT));


        _electrodeR.pLabelItem = new QGraphicsSimpleTextItem("ROI",_electrodeR.pRegionPathItem);
        QFont f=font();
        f.setPixelSize(20);
        _electrodeR.pLabelItem->setFont(f);
        _electrodeR.pLabelItem->setPen(QPen(Qt::red));
        _electrodeR.pLabelItem->setBrush(QBrush(Qt::white));

        for (int i=0;i<_electrodeR._nodes.count();++i)
        {
            ui->pDisplay->scene()->addItem(_electrodeR._nodes.at(i).pVisItem);
            _electrodeR.pRegionGroupItem->addToGroup(_electrodeR._nodes.at(i).pVisItem);
            _electrodeR._nodes.at(i).pVisItem->setParentItem(_electrodeR.pRegionPathItem);
            connect(_electrodeR._nodes.at(i).pVisItem,SIGNAL(IwasMoved(int)),this,SLOT(electrodeHandleItemMoved(int)));
        }

        ui->pDisplay->scene()->addItem(_electrodeR.pRegionGroupItem);
        _electrodeR.pRegionPathItem->setPos(_electrodeR.pRegionPathItem->mapFromScene(ui->pDisplay->mapToScene(ui->pDisplay->mapFromGlobal(QCursor::pos())))-QPointF(50,0));

    }

    _electrodeR.updatePath(ui->pShape->currentIndex());
    QPen pen(Qt::red);
    pen.setWidth(3);
    _electrodeR.pRegionPathItem->setPen(pen);


    if (_electrodeR.pRegionGroupItem)
    {
        ui->pDisplay->scene()->destroyItemGroup(_electrodeR.pRegionGroupItem);
        _electrodeR.pRegionGroupItem=NULL;
    }

}


void rdrMainDlg::on_pSettings_clicked()
{
    if (pSettingsWdgt==nullptr) pSettingsWdgt = new xrSettingsWdgt(this);
    if (!pSettingsWdgt->isVisible()) pSettingsWdgt->show();
    else pSettingsWdgt->hide();
    pSettingsWdgt->move((width()-pSettingsWdgt->width())/2,(height()-pSettingsWdgt->height())/2);
    pSettingsWdgt->raise();
}

