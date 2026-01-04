
#ifndef GAME_H
#define GAME_H

#include <QString>
#include <QWidget>
#include <vector>

using std::vector;

enum Suit { HEARTS, DIAMONDS, CLUBS, SPADES };
enum Rank { ACE = 1, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING };
enum Side { FRONT, BACK };

struct Card {
    Suit suit;
    Rank rank;
    Side side;
    QString toString() const;
};

enum PileType { STOCK, WASTE, STACK, TABLE };

struct Pile {
    PileType type;
    int index = -1;
    vector<Card> cards;
    QString toString() const;
};

enum GameStatus { STOPPED, RUNNING, WON, LAUNCHING };

struct GameState {
    Pile stock;
    Pile waste;
    vector<Pile> stacks;
    vector<Pile> tables;

    void dump() const;
};

class Game : public QObject
{
    Q_OBJECT

    GameState m_state;
    vector<GameState> m_history;
    size_t m_historyPointer = 0;

  public:
    Game(QObject *parent = nullptr);
    ~Game() = default;

    const GameState &state() const { return m_state; }
    void resetGame();

    // Modifier methods for moving cards
    Pile *getPileContainingCard(const Card &card);
    Pile *getPile(PileType type, int index);
    bool moveCardsToPile(const vector<Card> &cards, Pile *sourcePile, Pile *destPile);
    void handleStockCardClick();
    void recycleWasteToStock();

    // Undo/redo support
    void saveState();
    bool canUndo() const;
    bool canRedo() const;
    void undo();
    void redo();
};

const int CARD_WIDTH = 100;
const int CARD_HEIGHT = 150;
const int CARD_XDISTANCE = 20;

QPoint pileTypeAnchorPoint(PileType type);
int stackingDistance(PileType type);

#endif // GAME_H
