
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

enum PileType { Stock, Waste, Stack, Table };

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

  public:
    Game(QObject *parent = nullptr);
    ~Game() = default;

    const GameState &state() const { return m_state; }
    void resetGame();
};

#endif // GAME_H
