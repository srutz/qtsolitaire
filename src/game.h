
#ifndef GAME_H
#define GAME_H

#include "gamestate.h"
#include <QObject>
#include <QPoint>
#include <QString>
#include <vector>

using std::vector;
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
    void setState(const GameState &state);
    void resetGame();

    // Modifier methods for moving cards
    Pile *getPileContainingCard(const Card &card);
    Pile *getPile(PileType type, int index);
    bool moveCardsToPile(const vector<Card> &cards, Pile *sourcePile, Pile *destPile);
    void handleStockCardClick();
    void recycleWasteToStock();

    // Undo/redo support
    void pushState();
    bool canUndo() const;
    bool canRedo() const;
    void undo();
    void redo();

    // Persistence
    void saveToFile() const;
    void loadFromFile();
};

const int CARD_WIDTH = 100;
const int CARD_HEIGHT = 150;
const int CARD_XDISTANCE = 20;

QPoint pileTypeAnchorPoint(PileType type);
int stackingDistance(PileType type);

#endif // GAME_H
