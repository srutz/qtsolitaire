#include "game.h"
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QStandardPaths>
#include <algorithm>
#include <random>

using namespace std;

void Game::pushState()
{
    // Remove any states after the current pointer (if we're not at the end)
    if (m_history.size() > 0 && m_historyPointer < m_history.size() - 1) {
        m_history.erase(m_history.begin() + m_historyPointer + 1, m_history.end());
    }

    // Add current state to history
    m_history.push_back(m_state);
    m_historyPointer = m_history.size() - 1;

    // history limit
    const int MAX_HISTORY = 1'000;
    if (m_history.size() > MAX_HISTORY) {
        m_history.erase(m_history.begin());
        m_historyPointer--;
    }
    emit undoRedoStateChanged();
}

void Game::setState(const GameState &state)
{
    m_state = state;
    m_history.clear();
    m_historyPointer = 0;
    pushState();
}

void Game::setMode(GameMode mode)
{
    if (m_mode == mode) {
        return;
    }
    m_mode = mode;
    emit modeChanged();
}

bool Game::canUndo() const { return m_historyPointer > 0; }

bool Game::canRedo() const { return m_historyPointer < m_history.size() - 1; }

void Game::undo()
{
    if (canUndo()) {
        m_historyPointer--;
        m_state = m_history[m_historyPointer];
        emit undoRedoStateChanged();
    }
}

void Game::redo()
{
    if (canRedo()) {
        m_historyPointer++;
        // qDebug() << "oldstate before redo:" << m_state.toJson();
        m_state = m_history[m_historyPointer];
        // qDebug() << "newstate after  redo:" << m_state.toJson();
        emit undoRedoStateChanged();
    }
}

void Game::saveToFile() const
{
    QString homeDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString filePath = homeDir + "/.qtsolitaire.json";
    QJsonObject obj = m_state.toJson();
    QFile saveFile(filePath);
    if (saveFile.open(QIODevice::WriteOnly)) {
        saveFile.write(QJsonDocument(obj).toJson());
        saveFile.close();
    }
}

void Game::loadFromFile()
{
    QString homeDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString filePath = homeDir + "/.qtsolitaire.json";
    QFile loadFile(filePath);
    if (loadFile.exists() && loadFile.open(QIODevice::ReadOnly)) {
        QByteArray data = loadFile.readAll();
        QJsonDocument loadDoc(QJsonDocument::fromJson(data));
        if (!loadDoc.isNull()) {
            m_state = GameState::fromJson(loadDoc.object());
            m_history.clear();
            m_historyPointer = 0;
            pushState();
        }
        loadFile.close();
    }
}

Pile *Game::getPileContainingCard(const Card &card)
{
    for (const auto &c : m_state.stock.cards) {
        if (c.suit == card.suit && c.rank == card.rank) {
            return &m_state.stock;
        }
    }
    for (const auto &c : m_state.waste.cards) {
        if (c.suit == card.suit && c.rank == card.rank) {
            return &m_state.waste;
        }
    }
    for (auto &stack : m_state.stacks) {
        for (const auto &c : stack.cards) {
            if (c.suit == card.suit && c.rank == card.rank) {
                return &stack;
            }
        }
    }
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

vector<Card> Game::moveCardsToPile(const vector<Card> &cards, Pile *sourcePile, Pile *destPile)
{
    if (sourcePile == nullptr || destPile == nullptr || cards.empty()) {
        return {};
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
        return {};
    }

    // Verify that all cards to move are consecutive in the source pile
    if (startIndex + cards.size() > sourcePile->cards.size()) {
        return {};
    }

    vector<Card> toMove;
    // Move cards to destination pile
    for (const auto &card : cards) {
        destPile->cards.push_back(card);
        toMove.push_back(card);
    }

    // Remove cards from source pile
    sourcePile->cards.erase(sourcePile->cards.begin() + startIndex, sourcePile->cards.begin() + startIndex + cards.size());

    // If source pile is a TABLE and has cards remaining, flip the top card to FRONT
    if (sourcePile->type == TABLE && !sourcePile->cards.empty()) {
        sourcePile->cards.back().side = FRONT;
    }

    // Save state to undostack
    pushState();
    return toMove;
}

vector<Card> Game::handleStockCardClick()
{
    vector<Card> movedCards;
    if (!m_state.stock.cards.empty()) {
        // Save state before making changes
        pushState();
        // Take the top card from stock
        Card topCard = m_state.stock.cards.back();
        m_state.stock.cards.pop_back();
        // Flip it to front
        topCard.side = FRONT;
        // Put it on waste
        m_state.waste.cards.push_back(topCard);
        movedCards.push_back(topCard);
    }
    return movedCards;
}

vector<Card> Game::handleTableCardClick(Pile *pile)
{
    if (pile->cards.empty()) {
        return {};
    }
    // find a matching non empty stack
    for (auto &stack : m_state.stacks) {
        const auto &topTableCard = pile->cards.back();
        auto ace = topTableCard.rank == ACE;
        if (ace && stack.cards.empty()) {
            auto cards = moveCardsToPile({topTableCard}, pile, &stack);
            return cards;
        } else if (stack.cards.empty()) {
            continue;
        }

        const auto &topStackCard = stack.cards.back();
        if (topTableCard.suit == topStackCard.suit && topTableCard.rank == topStackCard.rank + 1) {
            auto cards = moveCardsToPile({topTableCard}, pile, &stack);
            return cards;
        }
    }
    return {};
}

void Game::recycleWasteToStock()
{
    // Save state before making changes
    pushState();

    // Move all cards from waste back to stock with back side
    while (!m_state.waste.cards.empty()) {
        Card card = m_state.waste.cards.back();
        m_state.waste.cards.pop_back();
        card.side = BACK;
        m_state.stock.cards.push_back(card);
    }
}

QPoint pileTypeAnchorPoint(PileType type)
{
    auto left = 80;
    auto topLine = 50;
    auto bottomLine = 330;
    switch (type) {
    case STOCK:
        return QPoint(left, topLine);
    case WASTE:
        return QPoint(left + CARD_WIDTH + CARD_XDISTANCE, topLine);
    case STACK:
        return QPoint(left + 3 * (CARD_WIDTH + CARD_XDISTANCE), topLine);
    case TABLE:
        return QPoint(left, bottomLine);
    default:
        return QPoint(0, 0);
    }
}

int stackingDistance(PileType type)
{
    switch (type) {
    case TABLE:
        return 21;
    default:
        return 3;
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
        .arg(static_cast<int>(type))
        .arg(index)
        .arg(cards.size())
        .arg(cardStrs.join(", "));
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

    // Reset undo/redo history
    m_history.clear();
    m_history.push_back(m_state);
    m_historyPointer = 0;
}
