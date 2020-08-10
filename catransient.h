#ifndef CATRANSIENT_H
#define CATRANSIENT_H

#include "xr_toolfunctions.h"
#include <QString>
#include <QVector>
#include "rdrmaindlg.h"

class caTransient: public QObject
{
Q_OBJECT
public:
    caTransient ():QObject(){};
    void calculateValues(const DATA_COMTAINER& p,int _min1, int _min2, int _max);

    // time index
    int lmin20 = -1;
    int rmin20 = -1;
    int lmid50 = -1;
    int rmid50 = -1;
    int lmax90 = -1;
    int rmax90 = -1;
    int lmin = 0;
    int rmin = 0;
    int _maxabs = 0;
    float timeToPeak,  decayR2, riseR2, descSlope, ascSlope, patHalfMax, transTime, decayTime, riseTime;
    QVector <float> decay, rise;
    // define to string
    QString toString();
signals:
    void message (const QString& );


};

#endif // CATRANSIENT_H
