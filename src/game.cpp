#include "game.h"
#include <QDebug>
#include <algorithm>
#include <random>

using namespace std;

QPoint pileTypeAnchorPoint(PileType type)
{
    switch (type) {
    case STOCK:
        return QPoint(50, 50);
    case WASTE:
        return QPoint(50 + CARD_WIDTH + CARD_XDISTANCE, 50);
    case STACK:
        return QPoint(50 + 3 * (CARD_WIDTH + CARD_XDISTANCE), 50);
    case TABLE:
        return QPoint(50, 400);
    default:
        return QPoint(0, 0);
    }
}

int stackingDistance(PileType type)
{
    switch (type) {
    case TABLE:
        return 15;
    default:
        return 4;
    }
}

QString Card::toString() const
{
    QString suit;
    switch (this->suit) {
    case HEARTS:
        suit = "H";
        break;
    case DIAMONDS:
        suit = "D";
        break;
    case CLUBS:
        suit = "C";
        break;
    case SPADES:
        suit = "S";
        break;
    }

    QString rank;
    switch (this->rank) {
    case ACE:
        rank = "A";
        break;
    case TWO:
        rank = "2";
        break;
    case THREE:
        rank = "3";
        break;
    case FOUR:
        rank = "4";
        break;
    case FIVE:
        rank = "5";
        break;
    case SIX:
        rank = "6";
        break;
    case SEVEN:
        rank = "7";
        break;
    case EIGHT:
        rank = "8";
        break;
    case NINE:
        rank = "9";
        break;
    case TEN:
        rank = "10";
        break;
    case JACK:
        rank = "J";
        break;
    case QUEEN:
        rank = "Q";
        break;
    case KING:
        rank = "K";
        break;
    }
    return suit + rank;
}

QString Pile::toString() const
{
    QStringList cardStrs;
    for (const auto &card : cards) {
        cardStrs << card.toString() + ((card.side == FRONT) ? "*" : "_");
    }
    return QString("Pile(type=%1, index=%2, size=%3, cards=[%4])")
        .arg(type)
        .arg(index)
        .arg(cards.size())
        .arg(cardStrs.join(", "));
}

void GameState::dump() const
{

    qDebug() << "stock:" << stock.toString();
    qDebug() << "waste:" << waste.toString();
    for (const auto &stack : stacks) {
        qDebug() << "stack:" << stack.toString();
    }
    for (const auto &table : tables) {
        qDebug() << "table:" << table.toString();
    }
}

Game::Game(QObject *parent) : QObject(parent) { resetGame(); }

void Game::resetGame()
{
    vector<Suit> suits{HEARTS, DIAMONDS, CLUBS, SPADES};
    vector<Rank> ranks{ACE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING};

    vector<Card> deck;
    for (const auto suit : suits) {
        for (const auto rank : ranks) {
            deck.push_back(Card{suit, rank, BACK});
        }
    }
    // shuffle here
    shuffle(deck.begin(), deck.end(), mt19937{std::random_device{}()});

    m_state.stock = {.type = STOCK, .index = -1, .cards = {}};
    m_state.waste = {.type = WASTE, .index = -1, .cards = {}};

    m_state.stacks.clear();
    for (auto i = 0; i < 4; i++) {
        m_state.stacks.push_back({.type = STACK, .index = i, .cards = {}});
    }

    m_state.tables.clear();
    for (auto i = 0; i < 7; i++) {
        // Extract i+1 cards from the deck
        vector<Card> content(deck.begin(), deck.begin() + i + 1);
        // Remove those cards from the deck
        deck.erase(deck.begin(), deck.begin() + i + 1);

        // The last card should be face up
        if (!content.empty()) {
            content.back().side = FRONT;
        }
        m_state.tables.push_back({.type = TABLE, .index = i, .cards = content});
    }

    // Remaining cards go to the stock
    this->m_state.stock.cards = deck;
}
