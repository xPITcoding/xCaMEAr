#ifndef RDR_CALC_H
#define RDR_CALC_H
#include "alglib/src/stdafx.h"
#include "alglib/src/interpolation.h"

#include "rdrmaindlg.h"

void nonLinearFit(DATA_COMTAINER *data, caTransient *transient, QVector <float> &values, QVector <float>& parameters, float& fitQuality, int fitFunction, int dir );

class rdr_calc
{
public:
    rdr_calc();
};

#endif // RDR_CALC_H
