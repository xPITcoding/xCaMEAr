#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "ui_rdrmaindlg.h"
#include "rdrmaindlg.h"
#include "catransient.h"
#include "../3rd_party/Alglib/src/interpolation.h"
#include "../3rd_party/Alglib/src/stdafx.h"
#include <QFileInfoList>
#include <QTextStream>
using namespace alglib;


void rdrMainDlg::findMaxPositions(QVector<float> values, QVector<int> &_peaks,int minFrame,int maxFrame, float lvlFac, int minIntervalFrames)
{
    if (values.length()>2)
    {
        float _minVal,_maxVal;
        _minVal=_maxVal=values.at(0);
        for (int i=minFrame;i<maxFrame;++i)
        {
            _minVal=std::min(_minVal,values.at(i));
            _maxVal=std::max(_maxVal,values.at(i));
        }

        float _lvlFac = lvlFac;
        float _lvl = _minVal+(_maxVal-_minVal)*_lvlFac;
        bool _startPntFound=false;
        QVector <int> _region;
        for (int i=minFrame+1;i<maxFrame-1;++i)
        {
            if (values.at(i)>_lvl && values.at(i-1)<_lvl)
            {
                _startPntFound=true;
                _region.clear();
            }
            if (_startPntFound) _region.append(i);
            if (values.at(i)>_lvl && values.at(i+1)<_lvl && _startPntFound)
            {
                if (_region.count()>=minIntervalFrames)
                {
                    // endpoint found
                    float _maxBrightnessInRegion=0.0f;
                    int _maxBrightnessPos=-1;
                    for (int j=0;j<_region.length();++j)
                        if (values.at(_region.at(j))>_maxBrightnessInRegion)
                        {
                            _maxBrightnessPos=_region.at(j);
                            _maxBrightnessInRegion=values.at(_region.at(j));
                        }
                    if (_maxBrightnessPos!=-1)
                        _peaks.append(_maxBrightnessPos);
                }
                _startPntFound=false;
            }
        }
    }
}
void rdrMainDlg::findMinPositions(QVector<float> values, QVector<int> _peaks, QVector <int> &_minPositions,int minFrame,int maxFrame)
{
    _minPositions.clear();
    //find min and max of the curve
    if (values.length()>2)
    {

        for (long maxId=0;maxId<_peaks.count();++maxId)
        {
            bool _lastMinFound=false;
            float _minVal;
            int _minPos;
            long j=_peaks.at(maxId);
            while (j>=minFrame && (maxId==0 || j>_peaks.at(maxId-1)))
            {
                if (_lastMinFound==false)
                {
                    _lastMinFound=true;
                    _minVal=values.at(j);
                    _minPos=j;
                }
                else {
                    if (values.at(j)<_minVal)
                    {
                        _minVal=values.at(j);
                        _minPos=j;
                    }
                }
                --j;
            }

            _minPositions.append(_minPos);
        }

        bool _lastMinFound=false;
        float _minVal;
        int _minPos;
        long j=_peaks.at(_peaks.count()-1);
        while (j<maxFrame)
        {
            if (_lastMinFound==false)
            {
                _lastMinFound=true;
                _minVal=values.at(j);
                _minPos=j;
            }
            else {
                if (values.at(j)<_minVal)
                {
                    _minVal=values.at(j);
                    _minPos=j;
                }
            }
            ++j;
        }

        _minPositions.append(_minPos);

    }
}



void rdrMainDlg::getIntervals(QVector<float> values, caTransient& transient, QVector<float> &intervalY, QVector<float> &intervalX, int dir)
{
    float point = 0;
    switch (dir) {
    case down:
        //get values from the right 90% max point to the next absolute transient minimum
        for(int i=transient.rmax90; i<transient.rmin20; i++)
        {
            intervalY.append(values.at(i));
            intervalX.append(point/data._frameRate*1000.0f);
            point++;
        }
        break;
    case up:
        //get values from the left 20% min point to the next absolute transient maximum
        for(float i=transient.lmin20; i<transient._maxabs; i++)
        {
            intervalY.append(values.at(i));
            intervalX.append(point/data._frameRate*1000.0f);
            point++;
        }
        break;
    }

}

void rdrMainDlg::mean (DATA_COMTAINER *p)
{
    try
    {
        bool error = catchError(p);
        if (p->transList.count()==0 | error ==true ) return;

        // Sugar was here and no princess :(((((((((

        float transTimeAv= 0.0f;
        float tToPeakAv= 0.0f;
        float riseR2Av=0.0f;
        float decayR2Av = 0.0f;
        float ascSlopeAv= 0.0f;
        float descSlopeAv=0.0f;
        float timeAtHalfMaxAv =0.0f;
        float risetimeAv =0.0f;
        float decaytimeAv =0.0f;
        float rt50av =0.0f;// averages
        QVector <float> riseAV(3,0.0f),decayAv(1,0.0f);
        // averages
        for (int i=0; i<p->transList.size(); i++)
        {
            transTimeAv+= p->transList.at(i)->transTime;
            tToPeakAv+= p->transList.at(i)->timeToPeak;
            riseR2Av += p->transList.at(i)->riseR2;
            decayR2Av += p->transList.at(i)->decayR2;
            ascSlopeAv += p->transList.at(i)->ascSlope;
            descSlopeAv +=p->transList.at(i)->descSlope;
            timeAtHalfMaxAv+=p->transList.at(i)->patHalfMax;
            riseAV[0] += p->transList.at(i)->rise[0];
            riseAV[1] += p->transList.at(i)->rise[1];
            riseAV[2] += p->transList.at(i)->rise[2];
            decayAv[0] +=p->transList.at(i)->decay[0];
            risetimeAv+=p->transList.at(i)->riseTime;
            decaytimeAv+=p->transList.at(i)->decayTime;
            rt50av+=p->transList.at(i)->rt50;
        }

        transTimeAv/=(float)p->transList.size();
        tToPeakAv/=(float)p->transList.size();
        riseR2Av/=(float)p->transList.size();
        decayR2Av/=(float)p->transList.size();
        ascSlopeAv/=(float)p->transList.size();
        descSlopeAv/=(float)p->transList.size();
        timeAtHalfMaxAv/=(float)p->transList.size();

        riseAV[0]/=(float)p->transList.size();
        riseAV[1]/=(float)p->transList.size();
        riseAV[2]/=(float)p->transList.size();
        decayAv[0]/=(float)p->transList.size();
        risetimeAv/=(float)p->transList.size();
        decaytimeAv/=(float)p->transList.size();
        rt50av/=(float)p->transList.size();

        fillTable("avg", transTimeAv, tToPeakAv, riseAV, riseR2Av, decayAv, decayR2Av, ascSlopeAv, descSlopeAv, timeAtHalfMaxAv, risetimeAv, decaytimeAv, rt50av);
    }
    catch (...)
    {
        QVector <float> error(1,0.0f);
        fillTable("avg", 0, 0, error, 0, error, 0, 0, 0, 0, 0, 0,0);

    }

}

