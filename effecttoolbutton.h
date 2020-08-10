#ifndef EFFECTTOOLBUTTON_H
#define EFFECTTOOLBUTTON_H

// provides single or permanent flashing of a toolbutton in red = warning, or green = to raise attention

#include <QObject>
#include <QToolButton>
#include <QGraphicsColorizeEffect>
#include <QPropertyAnimation>

enum ETB_STATUS
{
    ETBS_STANDARD,
    ETBS_WARNING,
    ETBS_ATTENTION
};

class EffectToolButton : public QToolButton
{
Q_OBJECT

public:
    EffectToolButton(QWidget* parent=NULL);
    ETB_STATUS status(){return _status;}
    void status(const ETB_STATUS& s);

protected slots:
    void removeEffectFromBtn();
    void cycle();
    void stopAnimation();

protected:
    ETB_STATUS _status;
    QGraphicsColorizeEffect *pBtnEffect;
    QPropertyAnimation *paAnimation;
    QTimer *pTimer;
};

#endif // EFFECTTOOLBUTTON_H
