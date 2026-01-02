
#include "pileitem.h"
#include <QDebug>
#include <QPainter>

PileItem::PileItem(const Pile &pile, QGraphicsItem *parent) : QGraphicsItem(parent), m_pile(pile) {}

QRectF PileItem::boundingRect() const { return QRectF(0, 0, 100, 150); }

void PileItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // set to smooth rendering
    painter->setRenderHint(QPainter::Antialiasing, true);

    // Use different color when highlighted
    if (m_highlighted) {
        painter->setBrush(QColor(100, 200, 100, 150)); // Green highlight
        painter->setPen(QPen(QColor(50, 255, 50), 3)); // Bright green border
    } else {
        painter->setBrush(QColor(107, 114, 128));
        painter->setPen(Qt::NoPen);
    }

    QRectF rect = boundingRect().adjusted(4, 4, -4, -4);
    painter->drawRoundedRect(rect, 4, 4);

    if (!m_highlighted) {
        painter->setPen(Qt::black);
        painter->drawText(boundingRect(), Qt::AlignCenter, QString("%1").arg(m_pile.type));
    }
}