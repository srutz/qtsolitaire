
#include "pileitem.h"
#include <QPainter>

PileItem::PileItem(const Pile &pile, QGraphicsItem *parent) : QGraphicsItem(parent), m_pile(pile) {}

QRectF PileItem::boundingRect() const { return QRectF(0, 0, 100, 150); }

void PileItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // Simple representation: draw a rectangle and the card's string
    painter->setBrush(Qt::yellow);
    painter->drawRect(boundingRect());
    painter->setPen(Qt::darkGreen);
    painter->drawText(boundingRect(), Qt::AlignCenter, QString("%1").arg(m_pile.type));
}