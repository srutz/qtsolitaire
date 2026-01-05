
#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <QJsonObject>
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
    QJsonObject toJson() const;

    static GameState fromJson(const QJsonObject &obj);
};

#endif // GAMESTATE_H
