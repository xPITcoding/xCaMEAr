#include "xr_toolfunctions.h"
#include "catransient.h"

#include "alglib/src/interpolation.h"
#include "alglib/src/stdafx.h"
using namespace alglib;

#include <QFileInfoList>
#include <QTextStream>

float* convertImageToFloatBuffer(QImage* I)
{
    float *res=(float*)malloc(I->width()*I->height()*sizeof (float));
    long offset=0;
    for (long y=0;y<I->height();++y)
        for (long x=0;x<I->width();++x)
            res[offset++]=qGray(I->pixel(x,y));
    return res;
}

float mean(const QVector<float>& v)
{
    float _buffer=0.0f;
    for (int i=0;i<v.count();++i)
        _buffer+=v[i];
    return _buffer/=(float)v.count();
}

bool catchError(DATA_COMTAINER *data)
{
    bool error = false;
    for(int tr = 0; tr<data->transList.count(); tr++){
        if(data->transList.at(tr)->lmin==-1 |
            data->transList.at(tr)->rmin==-1 |
            data->transList.at(tr)->_maxabs==-1 |
            data->transList.at(tr)->lmin20==-1 |
            data->transList.at(tr)->lmid50==-1 |
            data->transList.at(tr)->lmax90==-1 |
            data->transList.at(tr)->rmax90==-1 |
            data->transList.at(tr)->rmid50==-1 |
            data->transList.at(tr)->rmin20==-1)
            error = true;
    }
    return error;
}

void function_cx_1_func(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr)
{
    // this callback calculates f(c,x)=exp(-c0*sqr(x0))
    // where x is a position on X-axis and c is adjustable parameter
    func = c[1]*exp(-c[0]*x[0]);
}

void function_cx_2_func(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr)
{
    // this callback calculates f(c,x)=exp(-c0*sqr(x0))
    // where x is a position on X-axis and c is adjustable parameter
    func = -c[0]*pow(x[0],2)+c[1]*x[0]+c[2];
    //func joni = (c[1]*pow(x[0],2))+c[0];
}

void nonLinearFit(caTransient *transient, QList <QPointF> values, QVector <float>& parameters, float& fitQuality, int fitFunction, int dir )
{
    try
    {
        QVector <float> yvalues, xvalues;
        float _startTime = 0;
        switch (dir) {
        case down:
            _startTime = values.at(transient->rmax90).x();
            //get values from the right 90% max point to the next 20% minimum
            for(int i=transient->rmax90; i<transient->rmin20; i++)
            {
                yvalues.append(values.at(i).y());
                xvalues.append((values.at(i).x()-_startTime)/1000.0f); //convert to seconds
            }
            break;
        case up:
            //get values from the left 20% min point to the next absolute transient maximum
            _startTime = values.at(transient->_maxabs).x();
            for(float i=transient->lmin20; i<transient->_maxabs; i++)
            {
                yvalues.append(values.at(i).y());
                xvalues.append((values.at(i).x()-_startTime)/1000.0f); //convert to seconds
            }
            break;
        }


        //created Russian Arrays
        real_1d_array y; // intensity values scaled to 1
        y.setlength(yvalues.count());
        for (int i=0;i<yvalues.count();++i)
            y[i]=yvalues.at(i);

        real_2d_array x; // time values in ms
        x.setlength(xvalues.count(),1);
        for (int i=0;i<xvalues.count();++i)
        {
            x[i][0]=xvalues.at(i);
            x[i][1]=0;
        }



        // x(2)./exp(-x(1)*xdata^2)+x(0)

        // x(2)=0.039
        // x(1)=45.85 [tau]
        // x(0)=0.68

        real_1d_array c;
        double epsx = 0.00000001;
        ae_int_t maxits = 0;
        ae_int_t info;
        lsfitstate state;
        lsfitreport rep;
        double diffstep = 0.000001;

        //
        // Fitting without weights
        //
        switch (fitFunction) {
        case 1:
            //exponential decay
            //c = "[54.0, 0.018, 0.0]";
            c = "[1.0, 5.0]";
            lsfitcreatef(x, y, c, diffstep, state);
            lsfitsetcond(state, epsx, maxits);
            alglib::lsfitfit(state, function_cx_1_func);
            lsfitresults(state, info, c, rep);
            parameters.append(c.getcontent()[0]);
            fitQuality=rep.r2;
            break;

        case 2:
            //second degree polynome for the rise
            c = "[0.0, 0.08, 0.1]";
            //c = "[7.3, 1.57, 0.0]";
            lsfitcreatef(x, y, c, diffstep, state);
            lsfitsetcond(state, epsx, maxits);
            alglib::lsfitfit(state, function_cx_2_func);
            lsfitresults(state, info, c, rep);
            parameters.append(c.getcontent()[0]);
            parameters.append(c.getcontent()[1]);
            parameters.append(c.getcontent()[2]);
            fitQuality=rep.r2;
            break;
        }

        QFile f("/home/xpit/tmp/values.csv");
        if(f.open(QFile::WriteOnly))
        {
            QTextStream sy(&f);
            for(int i=0; i<xvalues.size(); i++)
            {
                sy << x[i][0] << ',' << y[i] << endl;
            }
            sy<< c[0] <<',' <<c[1] <<',' <<c[2] <<',' <<rep.r2 <<endl;
        }
        f.close();
    }
    catch (...)
    {
        parameters.append(-1);
        parameters.append(-1);
        parameters.append(-1);
        fitQuality=-1;
    }
    // error?

}
