
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
        painter->setBrush(QColor::fromRgb(0x9ca3af));
        painter->setPen(QColor::fromRgb(0x374151)); // Bright green border
    } else {
        painter->setBrush(Qt::NoBrush);
        painter->setPen(QColor::fromRgb(0x4b5563));
    }

    QRectF rect = boundingRect().adjusted(4, 4, -4, -4);
    painter->drawRoundedRect(rect, 4, 4);

    if (m_pile.type == STACK) {
        painter->setPen(QColor::fromRgb(0x374151));
        painter->drawText(boundingRect(), Qt::AlignCenter, QString("Stack"));
    }
}