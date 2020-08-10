#ifndef RDRREGIONVISUALIZATION_H
#define RDRREGIONVISUALIZATION_H

#include "rdrPathItem.h"
#include "rdrEllipseItem.h"

class rdrRegionVisualization
{
public:
    rdrRegionVisualization(){
        pRegionPathItem=NULL;
        pRegionGroupItem=NULL;
        pLabelItem=NULL;
    };
    rdrPathItem *pRegionPathItem;
    QGraphicsItemGroup *pRegionGroupItem;
    QGraphicsSimpleTextItem *pLabelItem;
    QVector <rdrRegionControlNode> _nodes;
    void updatePath(int shape);
    void setVisible(bool b)
    {
        if (pRegionPathItem) pRegionPathItem->setVisible(b);
        if (pRegionGroupItem) pRegionGroupItem->setVisible(b);
        if (pLabelItem) pLabelItem->setVisible(b);
        for (int i=0;i<_nodes.count();++i) _nodes[i].setVisible(b);
    }
};


#endif // RDRREGIONVISUALIZATION_H
