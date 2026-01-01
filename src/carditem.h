
#ifndef CARDITEM_H
#define CARDITEM_H

#include "game.h"
#include <QGraphicsItem>

class CardItem : public QGraphicsItem
{
    Card m_card;

  public:
    CardItem(const Card &card, QGraphicsItem *parent = nullptr);
    ~CardItem() override = default;

    const Card &card() const { return m_card; }
    void setCard(const Card &card)
    {
        m_card = card;
        update();
    }

    virtual QRectF boundingRect() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

#endif // CARDITEM_H
