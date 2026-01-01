

#ifndef PILEITEM_H
#define PILEITEM_H

#include "game.h"
#include <QGraphicsItem>

class PileItem : public QGraphicsItem
{
    Pile m_pile;

  public:
    PileItem(const Pile &pile, QGraphicsItem *parent = nullptr);
    ~PileItem() override = default;

    const Pile &pile() const { return m_pile; }

    virtual QRectF boundingRect() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

#endif // PILEITEM_H
