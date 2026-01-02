#include "game.h"
#include <QDebug>
#include <algorithm>
#include <random>

using namespace std;

Pile *Game::getPileContainingCard(const Card &card)
{
    // Check waste pile
    for (const auto &c : m_state.waste.cards) {
        if (c.suit == card.suit && c.rank == card.rank) {
            return &m_state.waste;
        }
    }

    // Check stacks
    for (auto &stack : m_state.stacks) {
        for (const auto &c : stack.cards) {
            if (c.suit == card.suit && c.rank == card.rank) {
                return &stack;
            }
        }
    }

    // Check tables
    for (auto &table : m_state.tables) {
        for (const auto &c : table.cards) {
            if (c.suit == card.suit && c.rank == card.rank) {
                return &table;
            }
        }
    }

    return nullptr;
}

Pile *Game::getPile(PileType type, int index)
{
    if (type == WASTE) {
        return &m_state.waste;
    } else if (type == STOCK) {
        return &m_state.stock;
    } else if (type == STACK) {
        for (auto &stack : m_state.stacks) {
            if (stack.index == index) {
                return &stack;
            }
        }
    } else if (type == TABLE) {
        for (auto &table : m_state.tables) {
            if (table.index == index) {
                return &table;
            }
        }
    }
    return nullptr;
}

bool Game::moveCardsToPile(const vector<Card> &cards, Pile *sourcePile, Pile *destPile)
{
    if (!sourcePile || !destPile || cards.empty()) {
        return false;
    }

    // Find the position of the first card to move in the source pile
    size_t startIndex = 0;
    bool found = false;
    for (size_t i = 0; i < sourcePile->cards.size(); ++i) {
        if (sourcePile->cards[i].suit == cards[0].suit && sourcePile->cards[i].rank == cards[0].rank) {
            startIndex = i;
            found = true;
            break;
        }
    }

    if (!found) {
        return false;
    }

    // Verify that all cards to move are consecutive in the source pile
    if (startIndex + cards.size() > sourcePile->cards.size()) {
        return false;
    }

    // Move cards to destination pile
    for (const auto &card : cards) {
        destPile->cards.push_back(card);
    }

    // Remove cards from source pile
    sourcePile->cards.erase(sourcePile->cards.begin() + startIndex, sourcePile->cards.begin() + startIndex + cards.size());

    // If source pile is a TABLE and has cards remaining, flip the top card to FRONT
    if (sourcePile->type == TABLE && !sourcePile->cards.empty()) {
        sourcePile->cards.back().side = FRONT;
    }

    return true;
}

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
