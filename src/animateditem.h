
#ifndef ANIMATEDITEM_H
#define ANIMATEDITEM_H

#include <QGraphicsItem>
#include <QVariantAnimation>

class AnimatedItem : public QGraphicsItem
{
    QVariantAnimation *m_posAnimation = nullptr;
    QVariantAnimation *m_pulseAnimation = nullptr;

  public:
    AnimatedItem(QGraphicsItem *parent = nullptr);
    ~AnimatedItem() override;

    void setPosAnimated(const QPointF &newPos, int duration = 250, int delay = 0);
    void pulse(int duration = 250, int delay = 0);
};

#endif // ANIMATEDITEM_H
