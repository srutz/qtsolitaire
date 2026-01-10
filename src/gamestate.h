
#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <QJsonObject>
#include <vector>

using std::vector;

/* the following enums are externalized, you break the save file format if you change
 * the ordinals */
enum Suit { HEARTS = 1, DIAMONDS, CLUBS, SPADES };
enum Rank { ACE = 1, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING };
enum Side { FRONT = 1, BACK };
enum PileType { STOCK = 1, WASTE, STACK, TABLE };

struct Card {
    Suit suit;
    Rank rank;
    Side side;
    QString toString() const;
};

struct Pile {
    PileType type;
    int index = -1;
    vector<Card> cards;
    QString toString() const;
};

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
