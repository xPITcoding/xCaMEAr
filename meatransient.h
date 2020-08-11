#ifndef MEATRANSIENT_H
#define MEATRANSIENT_H

#include <QPointF>
#include <QList>

class meatransient
{
public:
    meatransient(QList <QPointF> v, int maxPos, float gMax, float gMin);

    int lMin20;
    int rMin;
    float timeToDepol;
};

#endif // MEATRANSIENT_H
