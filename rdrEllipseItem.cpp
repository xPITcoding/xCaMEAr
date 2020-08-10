#include "rdrEllipseItem.h"

void rdrEllipseItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsEllipseItem::mouseMoveEvent(event);
    emit IwasMoved(pNode->_id);
}

