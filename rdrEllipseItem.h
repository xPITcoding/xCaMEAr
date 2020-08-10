#ifndef RDRELLIPSEITEM_H
#define RDRELLIPSEITEM_H

#include <QObject>
#include <QGraphicsEllipseItem>
#include <QPen>
#include <QBrush>

enum rdrRegionControlNodeType
{
    XLFLLCNT_INVALID,
    XLFLLCNT_POINT,
    XLFLLCNT_VIRTUAL_POINT
};

class rdrRegionControlNode;

class rdrEllipseItem:public QObject,public QGraphicsEllipseItem
{
    Q_OBJECT
public:
    rdrEllipseItem():QGraphicsEllipseItem(){};
    rdrEllipseItem(rdrRegionControlNode* pN,const QRectF &r):QGraphicsEllipseItem(r)
    {
        pNode = pN;
    };
signals:
    void IwasMoved(int);
protected:
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    rdrRegionControlNode *pNode;
};

class rdrRegionControlNode
{
public:
    int _id;
    rdrRegionControlNodeType _type;
    rdrEllipseItem *pVisItem = nullptr;
    rdrRegionControlNode()
    {
        _type = XLFLLCNT_INVALID;
    }
    rdrRegionControlNode(int i,const QPointF& p, const rdrRegionControlNodeType& t)
    {
        _type = t;
        // create shape, color, overlay cursor
        switch (_type)
        {
        case XLFLLCNT_POINT:
            pVisItem = new rdrEllipseItem(this,QRectF(-5,-5,10,10));
            pVisItem->moveBy(p.x(),p.y());
            pVisItem->setBrush(QBrush(QColor(Qt::red)));
            pVisItem->setPen(QPen(Qt::black));
            break;
        case XLFLLCNT_VIRTUAL_POINT:
            pVisItem = new rdrEllipseItem(this,QRectF(-5,-5,10,10));
            pVisItem->moveBy(p.x(),p.y());
            pVisItem->setBrush(QBrush(Qt::NoBrush));
            pVisItem->setPen(QPen(Qt::red));
            break;
        }
        pVisItem->setZValue(2);
        pVisItem->setFlags(QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsSelectable);
        _id=i;
    }
    void setVisible(bool b)
    {
        if (pVisItem) pVisItem->setVisible(b);
    }
};


#endif // RDRELLIPSEITEM_H
