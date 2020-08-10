#include "xrcolorsettingsitem.h"
#include "xrcolorpickdlg.h"
#include <QBrush>
#include <QPixmap>
#include <QPainter>

// display a frame with the correct color in the settings dialog
// a bit of gradient is applied so it looks less boring
xrColorSettingsItem::xrColorSettingsItem(const QColor& col,QWidget *parent) : QLabel(parent)
{
    setAutoFillBackground(true);
    setColor(col);
}

void xrColorSettingsItem::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (!_enabled) return;
    xrColorPickDlg dlg(0);
    dlg.setColor(_color);
    connect(&dlg,SIGNAL(colorPicked(const QColor& )),this,SLOT(setColor(const QColor&)));
    dlg.exec();
    QLabel::mouseDoubleClickEvent(event);
}

void xrColorSettingsItem::setColor(const QColor& col)
{
    QPixmap pix(width(),height());
    pix.fill(Qt::black);
    QPainter pain(&pix);
    QLinearGradient lg(10,10,width()-10,height()-10);
    lg.setColorAt(0.25,col);
    lg.setColorAt(1.0,Qt::black);
    QBrush brush(lg);
    brush.setStyle(Qt::LinearGradientPattern);
    pain.setBrush(brush);
    pain.drawRect(0,0,width(),height());
    pain.end();
    setPixmap(pix);
    _color = col;
    emit valueChanged();
}
