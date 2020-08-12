#ifndef RDRMAINDLG_H
#define RDRMAINDLG_H

#include <QDialog>
#include <QGraphicsPathItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsPathItem>
#include <QGraphicsItemGroup>
#include <QGraphicsSimpleTextItem>
#include <QPen>
#include <QGraphicsScene>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

#include "xrsettingswdgt.h"
#include "xr_toolfunctions.h"
#include "rdrCustomPixmapItem.h"
#include "rdrRegionVisualization.h"

#include "../3rd_party/Alglib/src/interpolation.h"
#include "../3rd_party/Alglib/src/stdafx.h"

using namespace alglib;
QT_BEGIN_NAMESPACE
namespace Ui { class rdrMainDlg; }
QT_END_NAMESPACE

using namespace QtCharts;

class rdrMainDlg : public QDialog
{
    Q_OBJECT

public:
    rdrMainDlg(QWidget *parent = nullptr);
    ~rdrMainDlg();

    DATA_COMTAINER* dataRef(){return &data;}

signals:
    void electrode_changed();

public slots:
    void printMessage(const QString&);

private slots:
    void on_pQuit_clicked();
    void updateStack(int value);
    void displaySlice(int i);
    void on_pSlider_valueChanged(int value);
    void on_pPlay_clicked();
    void play_next_Frame();
    void on_pPause_clicked();
    void createStandardPathItems();
    void createRegion(QPointF p);
    void electrodeHandleItemMoved(int);
    void zoom(int p);
    void on_pStart_clicked();
    void findMinPositions(QVector<float>, QVector<int>, QVector<int>&,int,int);             //values, maxPositions, minPositions [returned], leftMin, rightMin
    void findMaxPositions(QVector<float>, QVector<int>&,int,int, float lvlFac, int window);

    void getIntervals(QVector<float> values, caTransient& tr, QVector<float>& intervalY, QVector<float>& intervalX, int dir);
    void mean (DATA_COMTAINER *p);

    void on_pSettings_clicked();
    void fillTable(QString peak, float transTime, float timeToPeak, QVector <float> rise, float riseR2, QVector <float> decay, float decayR2, float ascSlope, float descSlope, float timeathalfmax, float risetime, float decaytime, float rt50);
    void on_pExport_clicked();
    void flatFielding();
    //void plotMEA();

    void on_pCalciumTB_clicked();

    void on_pBackgroundTB_clicked();

    void on_pElectrodesTB_clicked();

    void on_pDigitalTB_clicked();

    void on_pImport_clicked();

protected:
    QImage loadTIFFImage(const QString& fname);

signals:
    void message(const QString&);

private:
    Ui::rdrMainDlg *ui;

    DATA_COMTAINER data;

    rdrCustomPixmapItem* img = nullptr;

    QTimer* pMovieTimer = nullptr;

    bool _sceneActive;

    rdrRegionVisualization _backgrounR, _electrodeR;
    QChartView *pChartView;
    QLineSeries *pBkgSeries;
    QLineSeries *pRgSeries;
    xrSettingsWdgt *pSettingsWdgt=nullptr;

};


#endif // RDRMAINDLG_H


