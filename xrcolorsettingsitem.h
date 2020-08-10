/*
 * displays a color value in the settings dialog
 * CDullin xPIT 2020
 */

#ifndef XRCOLORSETTINGSITEM_H
#define XRCOLORSETTINGSITEM_H

#include <QLabel>

class xrColorSettingsItem : public QLabel
{
    Q_OBJECT
public:
    explicit xrColorSettingsItem(const QColor& col,QWidget *parent = nullptr);
    const QColor color(){return _color;}
    void setEnabled(const bool& b){_enabled=b;}

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
protected slots:
    void setColor(const QColor&);

signals:
    void valueChanged();
protected:
    QColor _color;
    bool _enabled = true;
};

#endif // XRCOLORSETTINGSITEM_H
