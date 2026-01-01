
#ifndef SOLITAIREWIDGET_H
#define SOLITAIREWIDGET_H

#include "carditem.h"
#include "game.h"
#include "pileitem.h"
#include <QGraphicsScene>
#include <QWidget>

class SolitaireWidget : public QWidget
{
    Q_OBJECT

    QGraphicsScene *m_scene = nullptr;
    Game m_game;
    vector<PileItem *> m_pileItems;
    vector<CardItem *> m_cardItems;

    CardItem *findCardItem(const Card &card) const;
    PileItem *findPileItem(PileType pileType, int index) const;

  public:
    explicit SolitaireWidget(QWidget *parent = nullptr);
    ~SolitaireWidget() override;

    void layoutGame();
};

#endif // SOLITAIREWIDGET_H
