#ifndef RDRPATHITEM_H
#define RDRPATHITEM_H

#include <QGraphicsItem>

enum RegionType
{
    RT_CIRCLE       = 0x00,
    RT_RECTANGLE    = 0x01
};

class rdrPathItem:public QGraphicsPathItem
{
public:
    rdrPathItem(QGraphicsItem* parent,const RegionType ty=RT_CIRCLE):QGraphicsPathItem(parent){_type=ty;};
    void setType(const RegionType& rt){_type=rt;}
    int regionType() {return _type;}
protected:
    RegionType _type;
    //virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event){};
};


#endif // RDRPATHITEM_H
