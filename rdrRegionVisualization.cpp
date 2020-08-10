#include "rdrRegionVisualization.h"

void rdrRegionVisualization::updatePath(int shape)
{
    QPainterPath lpath;
    lpath.moveTo(_nodes.at(0).pVisItem->pos());

    QRectF r (_nodes.at(1).pVisItem->pos().x()-(_nodes.at(1).pVisItem->pos().x()-_nodes.at(0).pVisItem->pos().x()),
              _nodes.at(0).pVisItem->pos().y()-(_nodes.at(1).pVisItem->pos().y()-_nodes.at(0).pVisItem->pos().y()),
              (_nodes.at(1).pVisItem->pos().x()-_nodes.at(0).pVisItem->pos().x())*2,
              (_nodes.at(1).pVisItem->pos().y()-_nodes.at(0).pVisItem->pos().y())*2);

    if(shape==0)
    {
        lpath.addEllipse(r);
        pRegionPathItem->setPath(lpath);
        pRegionPathItem->setType((RegionType)shape);
    }
    else
    {
        lpath.addRect(r);
        pRegionPathItem->setPath(lpath);
        pRegionPathItem->setType((RegionType)shape);
    }

    pLabelItem->setPos(pRegionPathItem->boundingRect().center()-pLabelItem->boundingRect().center());
}
