
#include "solitairewidget.h"
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QVBoxLayout>

SolitaireWidget::SolitaireWidget(QWidget *parent) : QWidget(parent)
{
    m_scene = new QGraphicsScene(this);
    m_scene->setBackgroundBrush(QColor(107, 114, 128));
    auto graphicsView = new QGraphicsView(m_scene, this);
    graphicsView->setObjectName("graphicsView");
    graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsView->setInteractive(true);
    graphicsView->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    graphicsView->setResizeAnchor(QGraphicsView::NoAnchor);
    graphicsView->setTransformationAnchor(QGraphicsView::NoAnchor);
    // graphicsView->setDragMode(QGraphicsView::NoDrag);

    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(graphicsView);
    setLayout(layout);

    // init the 52 carditems and the 13 piles and keep those objects around forever
    {
        m_pileItems.push_back(new PileItem(m_game.state().stock, this));
        m_pileItems.push_back(new PileItem(m_game.state().waste, this));
        for (const auto &stack : m_game.state().stacks) {
            m_pileItems.push_back(new PileItem(stack, this));
        }
        for (const auto &table : m_game.state().tables) {
            m_pileItems.push_back(new PileItem(table, this));
        }
        for (auto pileItem : m_pileItems) {
            m_scene->addItem(pileItem);
        }
    }
    for (const auto suit : {HEARTS, DIAMONDS, CLUBS, SPADES}) {
        for (const auto rank : {ACE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING}) {
            Card card{suit, rank, BACK};
            auto *cardItem = new CardItem(this, card);
            m_cardItems.push_back(cardItem);
            m_scene->addItem(cardItem);
        }
    }
    initPixmap();
    layoutGame(true);
}

SolitaireWidget::~SolitaireWidget()
{
    for (auto cardItem : m_cardItems) {
        delete cardItem;
    }
    for (auto pileItem : m_pileItems) {
        delete pileItem;
    }
}

void SolitaireWidget::initPixmap()
{
    if (m_pixmapLoaded) {
        return;
    }
    s_backPixmap.load(":/main/resources/cards/back.png");
    if (!s_backPixmap.isNull() && (s_backPixmap.width() != CARD_WIDTH || s_backPixmap.height() != CARD_HEIGHT)) {
        s_backPixmap = s_backPixmap.scaled(CARD_WIDTH, CARD_HEIGHT, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }
    m_pixmapLoaded = true;
}

CardItem *SolitaireWidget::findCardItem(const Card &card) const
{
    for (auto cardItem : this->m_cardItems) {
        if (cardItem->card().suit == card.suit && cardItem->card().rank == card.rank) {
            return cardItem;
        }
    }
    return nullptr;
}

PileItem *SolitaireWidget::findPileItem(PileType type, int index) const
{
    for (auto pileItem : this->m_pileItems) {
        if (pileItem->pile().type == type && pileItem->pile().index == index) {
            return pileItem;
        }
    }
    return nullptr;
}

void SolitaireWidget::layoutGame(bool delayed)
{
    const auto &state = m_game.state();
    qreal globalZValue = 0;
    auto layoutPile = [this, delayed, &globalZValue](const Pile &pile, QPoint pos) {
        auto *pileItem = findPileItem(pile.type, pile.index);
        pileItem->setPos(pos);
        auto yOffset = 0;
        auto yDistance = stackingDistance(pile.type);
        auto pileDuration = 500;
        qreal baseZValue = globalZValue;
        for (auto index = 0; const auto &card : pile.cards) {
            auto *cardItem = findCardItem(card);
            cardItem->setCard(card); // Update card with current side
            if (delayed) {
                cardItem->setPos(QPoint(-200, -200));
            }
            cardItem->setPosAnimated(
                QPoint(pos.x(), pos.y() + yOffset), 80, delayed ? 50 + (pileDuration / pile.cards.size() * index) : 0);

            // When dealing with delay, use global z-order to ensure proper layering during animation
            if (delayed) {
                cardItem->setZValue(globalZValue);
                globalZValue += 1;
            } else {
                cardItem->setZValue(baseZValue + index);
            }

            yOffset += yDistance;
            ++index;
        }

        if (!delayed) {
            globalZValue += pile.cards.size();
        }
    };
    layoutPile(state.stock, pileTypeAnchorPoint(PileType::STOCK));
    layoutPile(state.waste, pileTypeAnchorPoint(PileType::WASTE));

    auto stackPosition = pileTypeAnchorPoint(PileType::STACK);
    for (const auto &stack : state.stacks) {
        layoutPile(stack, stackPosition);
        stackPosition += QPoint(CARD_WIDTH + CARD_XDISTANCE, 0);
    }
    auto tablePosition = pileTypeAnchorPoint(PileType::TABLE);
    for (const auto &table : state.tables) {
        layoutPile(table, tablePosition);
        tablePosition += QPoint(CARD_WIDTH + CARD_XDISTANCE, 0);
    }
}

void SolitaireWidget::setHighlightedPile(PileItem *pile)
{
    if (m_highlightedPile == pile) {
        return;
    }

    if (m_highlightedPile != nullptr) {
        m_highlightedPile->setHighlighted(false);
    }
    if (m_highlightedCard != nullptr) {
        m_highlightedCard->setHighlighted(false);
        m_highlightedCard = nullptr;
    }

    m_highlightedPile = pile;

    if (m_highlightedPile != nullptr) {
        m_highlightedPile->setHighlighted(true);

        // Highlight the top card if the pile has cards
        Pile *actualPile = m_game.getPile(pile->pile().type, pile->pile().index);
        if (actualPile && !actualPile->cards.empty()) {
            const Card &topCard = actualPile->cards.back();
            m_highlightedCard = findCardItem(topCard);
            if (m_highlightedCard != nullptr) {
                m_highlightedCard->setHighlighted(true);
            }
        }
    }
}

PileItem *SolitaireWidget::findPileItemAt(const QPointF &scenePos)
{
    for (auto *pileItem : m_pileItems) {
        QRectF pileRect = pileItem->boundingRect().translated(pileItem->pos());

        // Extend the pile bounds to include all cards in the pile
        const Pile *pile = &pileItem->pile();
        Pile *actualPile = m_game.getPile(pile->type, pile->index);
        if (actualPile && !actualPile->cards.empty()) {
            int numCards = actualPile->cards.size();
            int yDistance = stackingDistance(pile->type);
            int extendedHeight = (numCards - 1) * yDistance + CARD_HEIGHT;
            pileRect.setHeight(extendedHeight);
        }

        if (pileRect.contains(scenePos)) {
            return pileItem;
        }
    }
    return nullptr;
}

Pile *SolitaireWidget::getPileForPileItem(PileItem *pileItem)
{
    if (pileItem == nullptr) {
        return nullptr;
    }
    return m_game.getPile(pileItem->pile().type, pileItem->pile().index);
}