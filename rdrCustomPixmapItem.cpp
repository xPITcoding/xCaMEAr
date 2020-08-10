#include "rdrCustomPixmapItem.h"
#include <QGraphicsSceneMouseEvent>

void rdrCustomPixmapItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button()==Qt::LeftButton)
    {
        emit createRegion(event->pos());
    }
}

void rdrCustomPixmapItem::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    emit zoom(event->delta());
}
