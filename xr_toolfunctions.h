#ifndef XR_TOOLFUNCTIONS_H
#define XR_TOOLFUNCTIONS_H

#include <QList>
#include <QImage>
#include <QVector>
#include <QMap>

class meatransient;
class caTransient;

enum direction
{
    up,
    down
};

struct DATA_COMTAINER
{
    bool _valid = false;
    QList <QImage*>  _frameLst;                  //calcium tiff stack
    QList <float*>  _framesInFloat;
    QList <QImage*>  _backgroundLst;             //backgound list
    QList <float*>  _backgroundInFloat;
    int             _dim[3];
    float           _frameRate;                  //frame rate per second
    float           _pixelSize;



    QVector <int> _minPos, _maxPos;
    QList <QPointF> _valuesCa;
    QList <QPointF> _valuesMEA;

    float minGlob, maxGlob;                        // global min and max
    double peakFreq=-1;                            //peak frequency parameter
    double peakIrregularity=-1;                    //standard deviation of peak frequency


    QList <caTransient*> transList;             //all calculated transients
    QList <meatransient*> transMEA;             //mea transients
    QMap <QString, QList<float>> _electrodeMap; //all electrodes recorded. Key(electrode number)=file, Key -1= time series
    QList <float> _strtPnt;                     // all starting points obtained from the timestamp data
    bool _startFound = false;                   //switch for start point
};

void nonLinearFit(caTransient *transient, QList <QPointF> values, QVector <float>& parameters, float& fitQuality, int fitFunction, int dir );
float mean(const QVector <float>& v);
bool catchError(DATA_COMTAINER *data);
float* convertImageToFloatBuffer(QImage* I);
QVector<float> movAvFilter(QVector <float>, float);
QVector<float> adaptiveMovAvFilter(QVector <float>, float);

#endif // XR_TOOLFUNCTIONS_H
