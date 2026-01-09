
#include "carditem.h"
#include "pileitem.h"
#include "solitairewidget.h"
#include <QCursor>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>

// Static back image loaded once and shared by all card instances
// static QPixmap s_backPixmap;
// static bool s_backPixmapLoaded = false;

CardItem::CardItem(SolitaireWidget *solitaireWidget, const Card &card, QGraphicsItem *parent)
    : AnimatedItem(parent), m_solitaireWidget(solitaireWidget), m_card(card)
{
    setAcceptedMouseButtons(Qt::LeftButton);
    setFlag(QGraphicsItem::ItemIsMovable, false); // We'll handle movement manually
}

void CardItem::setCard(const Card &card)
{
    m_card = card;
    update();
}

void CardItem::setHighlighted(bool highlighted)
{
    m_highlighted = highlighted;
    update();
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

    painter->setRenderHint(QPainter::Antialiasing, true);
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

    // Draw highlight border if highlighted
    if (m_highlighted) {
        painter->setBrush(QColor::fromRgba(0x4f9ca3af));
        painter->setPen(QColor::fromRgb(0x374151));

        QRectF rect = boundingRect(); //.adjusted(1, 1, 1, 1);
        painter->drawRoundedRect(rect, 4, 4);
    }
}

void CardItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    m_dragging = false;
    m_dragStartMousePos = event->scenePos();
    if (event->button() == Qt::LeftButton && m_card.side == FRONT) {
        m_dragStartPos = pos();

        // Find all cards in the same pile that are below this card
        auto &draggedCards = m_solitaireWidget->draggedCardItems();
        draggedCards.clear();

        // Find which pile this card belongs to and get all cards from this one down
        const auto &state = m_solitaireWidget->game().state();
        bool foundThisCard = false;

        auto findCardsInPile = [&](const Pile &pile) {
            for (size_t i = 0; i < pile.cards.size(); ++i) {
                const auto &card = pile.cards[i];
                if (card.suit == m_card.suit && card.rank == m_card.rank) {
                    foundThisCard = true;
                }
                if (foundThisCard) {
                    // Find the CardItem for this card and add to dragged list
                    for (auto *item : scene()->items()) {
                        if (auto *cardItem = dynamic_cast<CardItem *>(item)) {
                            if (cardItem->card().suit == card.suit && cardItem->card().rank == card.rank) {
                                draggedCards.push_back(cardItem);
                                break;
                            }
                        }
                    }
                }
            }
        };

        findCardsInPile(state.waste);
        for (const auto &stack : state.stacks) {
            if (!foundThisCard) {
                findCardsInPile(stack);
            }
        }
        for (const auto &table : state.tables) {
            if (!foundThisCard) {
                findCardsInPile(table);
            }
        }

        // Store original positions for all dragged cards
        m_draggedCardStartPositions.clear();
        for (auto *cardItem : draggedCards) {
            m_draggedCardStartPositions.push_back(cardItem->pos());
        }

        // Raise z-value for all dragged cards, maintaining their relative order
        for (size_t i = 0; i < draggedCards.size(); ++i) {
            draggedCards[i]->setZValue(1000 + i);
        }

        setCursor(Qt::OpenHandCursor);
        event->accept();
    } else if (event->button() == Qt::LeftButton) {
        // release event should survice
        event->accept();
    } else {
        event->ignore();
    }
}

void CardItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    auto &draggedCards = m_solitaireWidget->draggedCardItems();
    if (!draggedCards.empty() && draggedCards[0] == this) {
        m_dragging = true;
        QPointF delta = event->scenePos() - event->buttonDownScenePos(Qt::LeftButton);

        // Move all dragged cards together, maintaining their relative positions
        for (size_t i = 0; i < draggedCards.size(); ++i) {
            if (i < m_draggedCardStartPositions.size()) {
                draggedCards[i]->setPos(m_draggedCardStartPositions[i] + delta);
            }
        }

        // Check for pile highlighting
        PileItem *pileUnderCursor = m_solitaireWidget->findPileItemAt(event->scenePos());
        m_solitaireWidget->setHighlightedPile(pileUnderCursor);

        event->accept();
    } else {
        event->ignore();
    }
}

void CardItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    double dragDistance = QPointF(event->scenePos() - m_dragStartMousePos).manhattanLength();
    if (event->button() == Qt::LeftButton && !m_dragging && dragDistance < 5.0) {
        // Handle stock card click - flip and move to waste
        Pile *pile = m_solitaireWidget->game().getPileContainingCard(m_card);
        // qDebug() << "Clicked card:" << m_card.toString() << " in pile:" << (pile ? pile->toString() : "null");
        if (pile != nullptr && pile->type == STOCK) {
            m_solitaireWidget->game().handleStockCardClick();
            m_solitaireWidget->layoutGame();
            event->accept();
            return;
        } else if (pile != nullptr && !pile->cards.empty() && (pile->type == TABLE || pile->type == WASTE)) {
            // get topcard of table pile
            if (m_solitaireWidget->game().handleTableCardClick(pile)) {
                m_solitaireWidget->layoutGame();
                return;
            }
        }
    } else if (event->button() == Qt::LeftButton) {
        auto &draggedCards = m_solitaireWidget->draggedCardItems();
        if (!draggedCards.empty() && draggedCards[0] == this) {
            // Reset z-value for all dragged cards
            for (auto *cardItem : draggedCards) {
                cardItem->setZValue(0);
            }

            // Check if there's a pile under the cursor
            auto *destPileItem = m_solitaireWidget->findPileItemAt(event->scenePos());

            if (destPileItem != nullptr) {
                // Find the source pile
                auto *sourcePile = m_solitaireWidget->game().getPileContainingCard(draggedCards[0]->card());

                // Get destination pile
                auto *destPile = m_solitaireWidget->getPileForPileItem(destPileItem);
                if (!isValidMove(draggedCards, sourcePile, destPile)) {
                    // Invalid move, return cards to original positions
                    for (size_t i = 0; i < draggedCards.size(); ++i) {
                        if (i < m_draggedCardStartPositions.size()) {
                            draggedCards[i]->setPosAnimated(m_draggedCardStartPositions[i]);
                        }
                    }
                    m_solitaireWidget->setHighlightedPile(nullptr);
                    m_solitaireWidget->layoutGame();
                    draggedCards.clear();
                    setCursor(Qt::ArrowCursor);
                    event->accept();
                    return;
                }

                // push state and move them
                vector<Card> cardsToMove;
                for (auto *cardItem : draggedCards) {
                    cardsToMove.push_back(cardItem->card());
                }
                if (destPile != nullptr && sourcePile != nullptr && sourcePile != destPile) {
                    m_solitaireWidget->game().moveCardsToPile(cardsToMove, sourcePile, destPile);
                }
            }
            m_solitaireWidget->setHighlightedPile(nullptr);
            m_solitaireWidget->layoutGame();
            draggedCards.clear();
        }

        setCursor(Qt::ArrowCursor);
        event->accept();
    } else {
        event->ignore();
    }
}

bool CardItem::isValidMove(const vector<CardItem *> &draggedCardItems, Pile *sourcePile, Pile *destPile)
{
    if (sourcePile == nullptr || destPile == nullptr || draggedCardItems.empty()) {
        return false;
    } else if (sourcePile == destPile) {
        return false;
    } else if (destPile->type == STOCK) {
        return false;
    } else if (destPile->type == WASTE) {
        return false;
    } else if (destPile->type == STACK) {
        if (draggedCardItems.size() != 1) {
            return false;
        }
        const auto &card = draggedCardItems[0]->card();
        if (destPile->cards.empty()) {
            if (card.rank != ACE) {
                return false;
            }
        } else {
            const auto &topCard = destPile->cards.back();
            // Must be same suit and one rank higher
            if (card.suit != topCard.suit || card.rank != topCard.rank + 1) {
                return false;
            }
        }
    } else if (destPile->type == TABLE) {
        if (!destPile->cards.empty()) {
            // Now check the bottom card of the dragged sequence against the top card of the destination pile
            const auto &topDraggedCard = draggedCardItems[0]->card();
            const auto &topPileCard = destPile->cards.back();

            // Must be alternating colors and one rank lower
            auto isRed = [](Suit suit) { return suit == HEARTS || suit == DIAMONDS; };
            auto isBlack = [](Suit suit) { return suit == CLUBS || suit == SPADES; };

            auto alternatingColors = (isRed(topDraggedCard.suit) && isBlack(topPileCard.suit)) ||
                                     (isBlack(topDraggedCard.suit) && isRed(topPileCard.suit));

            auto validMove = alternatingColors && topDraggedCard.rank == topPileCard.rank - 1;
            if (!validMove) {
                return false;
            }
        }
    }
    return true;
}