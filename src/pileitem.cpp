
#include "pileitem.h"
#include "solitairewidget.h"
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>

PileItem::PileItem(const Pile &pile, QGraphicsItem *parent) : QGraphicsItem(parent), m_pile(pile) {}

PileItem::PileItem(const Pile &pile, SolitaireWidget *solitaireWidget, QGraphicsItem *parent)
    : QGraphicsItem(parent), m_pile(pile), m_solitaireWidget(solitaireWidget)
{
    setAcceptedMouseButtons(Qt::LeftButton);
}

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
        painter->setPen(QColor::fromRgb(0x374151));
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

void PileItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_pile.type == STOCK && m_solitaireWidget != nullptr) {
        // Check if stock pile is empty
        Pile *stockPile = m_solitaireWidget->game().getPile(STOCK, -1);
        if (stockPile != nullptr && stockPile->cards.empty()) {
            // Recycle waste back to stock
            m_solitaireWidget->game().recycleWasteToStock();
            m_solitaireWidget->layoutGame();
            event->accept();
            return;
        }
    }
    event->ignore();
}