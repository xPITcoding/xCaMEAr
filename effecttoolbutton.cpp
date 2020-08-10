#include "effecttoolbutton.h"

#include <QTimer>

EffectToolButton::EffectToolButton(QWidget* parent):QToolButton(parent)
{
    _status = ETBS_STANDARD;
    setFocusPolicy(Qt::NoFocus);
    pBtnEffect = new QGraphicsColorizeEffect(this);
    QColor endColor = QColor(47,61,68);
    pBtnEffect->setStrength(0.5);
    pBtnEffect->setEnabled(false);
    this->setGraphicsEffect(pBtnEffect);
    paAnimation = new QPropertyAnimation(pBtnEffect,"color",this);        // set parent is important
    paAnimation->setStartValue(QColor(Qt::red));
    paAnimation->setEndValue(endColor);
    paAnimation->setDuration(300);
    connect(paAnimation,SIGNAL(finished()),this,SLOT(removeEffectFromBtn()));

    pTimer = new QTimer();
    pTimer->setSingleShot(true);
    pTimer->setInterval(1500);
    connect(pTimer,SIGNAL(timeout()),this,SLOT(cycle()));

    connect(this,SIGNAL(clicked()),this,SLOT(stopAnimation()));

}

void EffectToolButton::stopAnimation()
{
    if (pBtnEffect) pBtnEffect->setEnabled(false);
    _status = ETBS_STANDARD;
    if (pTimer) pTimer->stop();
    if (paAnimation) paAnimation->stop();
}

void EffectToolButton::status(const ETB_STATUS &s)
{
    _status = s;
    switch (s)
    {
    case ETBS_STANDARD:
        stopAnimation();
        break;
    case ETBS_WARNING:
        pBtnEffect->setEnabled(true);
        paAnimation->setStartValue(QColor(Qt::red));
        paAnimation->setEndValue(QColor(47,61,68));
        paAnimation->setDuration(300);
        paAnimation->start();
        break;
    case ETBS_ATTENTION:
        pBtnEffect->setEnabled(true);
        pBtnEffect->setStrength(1);
        paAnimation->setStartValue(QColor(47,61,68));
        paAnimation->setKeyValueAt(0.5,QColor(Qt::green));
        paAnimation->setEndValue(QColor(47,61,68));
        paAnimation->setDuration(500);
        paAnimation->start();
        pTimer->start();
        break;
    }
}

void EffectToolButton::cycle()
{
    status(ETBS_ATTENTION);
}

void EffectToolButton::removeEffectFromBtn()
{
    QPropertyAnimation *paAnimation = dynamic_cast<QPropertyAnimation *>(sender());
    if (paAnimation)
    {
        QWidget *pWdgt = dynamic_cast<QWidget*>(paAnimation->parent());
        if (pWdgt)
        {
            QGraphicsEffect *pEffect=pWdgt->graphicsEffect();
            if (pEffect)
                pEffect->setEnabled(false);
        }
    }
    _status = ETBS_STANDARD;
}

