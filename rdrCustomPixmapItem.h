#ifndef RDRCUSTOMPIXMAPITEM_H
#define RDRCUSTOMPIXMAPITEM_H

#include <QGraphicsPixmapItem>
#include <QObject>

class rdrCustomPixmapItem: public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    rdrCustomPixmapItem():QObject(),QGraphicsPixmapItem(){};
signals:
    void createRegion(QPointF p);
    void zoom(int p);
protected:
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void wheelEvent(QGraphicsSceneWheelEvent *event) override;
};

#endif // RDRCUSTOMPIXMAPITEM_H
