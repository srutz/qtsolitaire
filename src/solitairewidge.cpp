
#include "solitairewidget.h"
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QVBoxLayout>

SolitaireWidget::SolitaireWidget(QWidget *parent) : QWidget(parent)
{
    m_scene = new QGraphicsScene(this);
    auto graphicsView = new QGraphicsView(m_scene, this);
    graphicsView->setObjectName("graphicsView");
    graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(graphicsView);
    setLayout(layout);

    // init the 52 carditems and keep those objects around forever

    for (const auto suit : {HEARTS, DIAMONDS, CLUBS, SPADES}) {
        for (const auto rank : {ACE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING}) {
            Card card{suit, rank, BACK};
            auto *cardItem = new CardItem(card);
            m_cardItems.push_back(cardItem);
            m_scene->addItem(cardItem);
        }
    }

    // auto *box1 = m_scene->addRect(50, 50, 100, 150, QPen(Qt::black), QBrush(Qt::red));

    m_game.state().dump();
    layoutGame();
}

SolitaireWidget::~SolitaireWidget()
{
    // delete points in m_cardItems
    for (auto cardItem : m_cardItems) {
        delete cardItem;
    }
}

CardItem *SolitaireWidget::findItem(const Card &card) const
{
    for (auto cardItem : this->m_cardItems) {
        if (cardItem->card().suit == card.suit && cardItem->card().rank == card.rank) {
            return cardItem;
        }
    }
    return nullptr;
}

void SolitaireWidget::layoutGame()
{
    const GameState &state = m_game.state();
    auto layoutPile = [this](const Pile &pile, double x, double y) {
        double yOffset = 0;
        for (const auto &card : pile.cards) {
            auto *item = findItem(card);
            item->setPos(x, y + yOffset);
            yOffset += 15;
        }
    };
    layoutPile(state.stock, 50, 50);
    layoutPile(state.waste, 200, 50);
    double stackX = 50;
    for (const auto &stack : state.stacks) {
        layoutPile(stack, stackX, 300);
        stackX += 120;
    }
    double tableX = 50;
    for (const auto &table : state.tables) {
        layoutPile(table, tableX, 600);
        tableX += 120;
    }
}
