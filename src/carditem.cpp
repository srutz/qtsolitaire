
#include "carditem.h"
#include "solitairewidget.h"
#include <QDebug>
#include <QPainter>

// Static back image loaded once and shared by all card instances
// static QPixmap s_backPixmap;
// static bool s_backPixmapLoaded = false;

CardItem::CardItem(SolitaireWidget *solitaireWidget, const Card &card, QGraphicsItem *parent)
    : QGraphicsItem(parent), m_solitaireWidget(solitaireWidget), m_card(card)
{
}

void CardItem::initPixmap()
{
    if (m_pixmapLoaded) {
        return;
    }
    QString rankStr;
    switch (m_card.rank) {
    case ACE:
        rankStr = "A";
        break;
    case TWO:
        rankStr = "2";
        break;
    case THREE:
        rankStr = "3";
        break;
    case FOUR:
        rankStr = "4";
        break;
    case FIVE:
        rankStr = "5";
        break;
    case SIX:
        rankStr = "6";
        break;
    case SEVEN:
        rankStr = "7";
        break;
    case EIGHT:
        rankStr = "8";
        break;
    case NINE:
        rankStr = "9";
        break;
    case TEN:
        rankStr = "0";
        break;
    case JACK:
        rankStr = "J";
        break;
    case QUEEN:
        rankStr = "Q";
        break;
    case KING:
        rankStr = "K";
        break;
    }

    QString suitStr;
    switch (m_card.suit) {
    case HEARTS:
        suitStr = "H";
        break;
    case DIAMONDS:
        suitStr = "D";
        break;
    case CLUBS:
        suitStr = "C";
        break;
    case SPADES:
        suitStr = "S";
        break;
    }

    auto imagePath = QString(":/main/resources/cards/%1%2.png").arg(rankStr).arg(suitStr);
    if (!m_pixmap.load(imagePath)) {
        qDebug() << "Failed to load card image:" << imagePath;
        qFatal("exiting");
    }

    // Scale the pixmap to fit the card dimensions if needed
    if (!m_pixmap.isNull() && (m_pixmap.width() != CARD_WIDTH || m_pixmap.height() != CARD_HEIGHT)) {
        m_pixmap = m_pixmap.scaled(CARD_WIDTH, CARD_HEIGHT, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }
    m_pixmapLoaded = true;
}

QRectF CardItem::boundingRect() const { return QRectF(0, 0, CARD_WIDTH, CARD_HEIGHT); }

void CardItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    initPixmap();

    // Draw the card image
    if (!m_pixmap.isNull()) {
        painter->drawPixmap(0, 0, m_card.side == FRONT ? m_pixmap : m_solitaireWidget->backPixmap());
    } else {
        // Fallback if image fails to load
        painter->setBrush(Qt::lightGray);
        painter->drawRect(boundingRect());
        // Debug text overlay
        painter->setPen(Qt::red);
        painter->setFont(QFont("Arial", 8));
        painter->drawText(
            boundingRect(), Qt::AlignTop | Qt::AlignLeft, m_card.toString() + (m_card.side == FRONT ? " (F)" : " (B)"));
    }
}
