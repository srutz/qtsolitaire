
#include "carditem.h"
#include <QPainter>

CardItem::CardItem(const Card &card, QGraphicsItem *parent) : QGraphicsItem(parent), m_card(card) {}

QRectF CardItem::boundingRect() const { return QRectF(0, 0, CARD_WIDTH, CARD_HEIGHT); }

void CardItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // Simple representation: draw a rectangle and the card's string
    painter->setBrush(Qt::lightGray);
    painter->drawRect(boundingRect());
    painter->setPen(Qt::black);
    painter->drawText(boundingRect(), Qt::AlignCenter, m_card.toString() + (m_card.side == FRONT ? " (Front)" : " (Back)"));
}