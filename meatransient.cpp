#include "meatransient.h"
#include "xr_toolfunctions.h"
#include <math.h>
#ifdef Q_OS_WINDOWS
#include <stdlib.h>
#include <windows.h>
#endif

using namespace std;

meatransient::meatransient(QList <QPointF> v, int maxPos, float gMax, float gMin)
{
    lMin20=maxPos;
    rMin=maxPos;

    float absMax = max(gMax,fabs(gMin));

    while((v.at(lMin20).y()/absMax)>0.20f)
        lMin20--;

    QVector <float> _meaValues;
    for (long i=0;i<v.count();++i)
        _meaValues.append(v[i].y());
    QVector <float> _fltrdMEA = movAvFilter(_meaValues, 50.0f);
    while ((_fltrdMEA.at(rMin)/absMax)>(_fltrdMEA.at(rMin+1)/absMax))
        rMin++;
    timeToDepol = (v.at(rMin).x()-v.at(lMin20).x())/1000.0f; //in seconds
}
