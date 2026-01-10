
#ifndef GAME_H
#define GAME_H

#include "gamestate.h"
#include <QObject>
#include <QPoint>
#include <QString>
#include <vector>

using std::vector;

enum GameMode { STOPPED = 1, RUNNING, WON, LAUNCHING };

class Game : public QObject
{
    Q_OBJECT

    GameMode m_mode = STOPPED;
    GameState m_state;
    vector<GameState> m_history;
    size_t m_historyPointer = 0;

  public:
    Game(QObject *parent = nullptr);
    ~Game() = default;

    const GameState &state() const { return m_state; }
    void setState(const GameState &state);

    GameMode mode() const { return m_mode; }
    void setMode(GameMode mode);

    void resetGame();

    // Modifier methods for moving cards
    Pile *getPileContainingCard(const Card &card);
    Pile *getPile(PileType type, int index);
    vector<Card> moveCardsToPile(const vector<Card> &cards, Pile *sourcePile, Pile *destPile);
    vector<Card> handleStockCardClick();
    vector<Card> handleTableCardClick(Pile *pile);

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

  signals:
    void modeChanged();
    void undoRedoStateChanged();
};

const int CARD_WIDTH = 100;
const int CARD_HEIGHT = 150;
const int CARD_XDISTANCE = 20;

QPoint pileTypeAnchorPoint(PileType type);
int stackingDistance(PileType type);

#endif // GAME_H
