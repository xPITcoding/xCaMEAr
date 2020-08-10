#include "catransient.h"
#include "xr_toolfunctions.h"

#include <QObject>
#include <QTextStream>
                                                        //index
// caTransient *pcaT = new caTransient(this,data, data._minPos.at(i), data._minPos.at(i+1), data._maxPos.at(i));

void caTransient::calculateValues(const DATA_COMTAINER& p,int _min1, int _min2, int _ma)
{

    emit message("Begin creation of transient:");
    lmin = _min1;
    rmin = _min2;
    _maxabs = _ma;
    for(quint16 i = lmin; i<=rmin; ++i)
    {
        if(p._valuesCa.at(i).y()>0.2 && lmin20 == -1)
            lmin20 = i;

        if(p._valuesCa.at(i).y()>0.5 && lmid50 == -1)
            lmid50 = i;

        if(p._valuesCa.at(i).y()>0.9 && lmax90 == -1)
            lmax90 = i;


    }
    /*
    if(lmin20 ==-1)     
        emit message("lmin20 not found");
    if(lmid50 ==-1)        
        emit message("lmid50 not found");
    if(lmax90 ==-1)
        emit message("lmax90 not found");
    */
    for (int i=rmin; i>=lmin; --i)
    {

        if(p._valuesCa.at(i).y()>0.2 && rmin20 == -1)
            rmin20 = i;

        if(p._valuesCa.at(i).y()>0.5 && rmid50 == -1)
            rmid50 = i;

        if(p._valuesCa.at(i).y()>0.9 && rmax90 == -1)
            rmax90 = i;

    }

    if(rmin20 ==-1)
        emit message("rmin20 not found");
    if(rmid50 ==-1)
        emit message("rmid50 not found");
    if(rmax90 ==-1)
        emit message("rmax90 not found");

    if (rmax90 != -1 && rmin20!=-1){
    // calculate decay
    nonLinearFit(this, p._valuesCa, decay, decayR2, 1, down);}
    else emit message("Decay not calculated");

    if (lmin20 != -1 && lmax90!=-1){
    // calculate rise
    nonLinearFit(this, p._valuesCa, rise, riseR2, 2, up);}
    else emit message("Rise not calculated");

    QFile f("/home/xpit/tmp/decay.csv");
    f.open(QFile::WriteOnly);
    QTextStream t(&f);
    float _startTime = p._valuesCa.at(rmax90).x();
    t << _startTime << Qt::endl;
    t << rmax90 << Qt::endl;
    t << rmin20 << Qt::endl;
    for(float i=rmax90; i<rmin20; i++)
    {
        t << QString("%1,%2\n").arg((p._valuesCa.at(i).x()-_startTime)/1000.0f).arg(p._valuesCa.at(i).y());
    }
    f.close();

    // calculate time to peak
    timeToPeak= (p._valuesCa.at(_maxabs).x()-p._valuesCa.at(lmin).x())/1000.0f; //in seconds

    if (rmax90 !=-1 && rmin20!=-1){
    //get linear slope of descending
    descSlope = (p._valuesCa.at(rmin20).y()-p._valuesCa.at(rmax90).y()) / ((p._valuesCa.at(rmin20).x()/1000.0f)-(p._valuesCa.at(rmax90).x())/1000.0f);} //in seconds
    else emit message("Descending slope not calculated");

    if (lmin20!=-1){
    //get linear slope of ascending
    ascSlope = (p._valuesCa.at(_maxabs).y()-p._valuesCa.at(lmin20).y()) / ((p._valuesCa.at(_maxabs).x()/1000.0f)-(p._valuesCa.at(lmin20).x()/1000.0f));} //in seconds
    else emit message("Ascending slope not calculated");

    if (rmid50!= -1 && lmid50!=-1){
    //peak width at half maximum
    patHalfMax = (p._valuesCa.at(rmid50).x()/1000.0f)-(p._valuesCa.at(lmid50).x()/1000.0f);} //in seconds
    else emit message("Time at half maximum not calculated");

    //calculate transient time
    transTime = (p._valuesCa.at(rmin).x()/1000.0f)-(p._valuesCa.at(lmin).x()/1000.0f); //in seconds

    if (rmin20!=-1){
    //calculate decay time
    decayTime =(p._valuesCa.at(rmin20).x()/1000.0f)-(p._valuesCa.at(_maxabs).x()/1000.0f);} //in seconds
    else emit message("Decay time not calculated");

     if (lmin20!=-1){
    //calculate rise time
    riseTime =(p._valuesCa.at(_maxabs).x()/1000.0f)-(p._valuesCa.at(lmin20).x()/1000.0f);} //in seconds
     else emit message("Rise time not calculated");

     emit message("End transient");

}
