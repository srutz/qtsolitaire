
#ifndef SOLITAIREWIDGET_H
#define SOLITAIREWIDGET_H

#include "carditem.h"
#include "game.h"
#include <QGraphicsScene>
#include <QWidget>

class SolitaireWidget : public QWidget
{
    Q_OBJECT

    QGraphicsScene *m_scene = nullptr;
    Game m_game;
    vector<CardItem *> m_cardItems;

    CardItem *findItem(const Card &card) const;

  public:
    explicit SolitaireWidget(QWidget *parent = nullptr);
    ~SolitaireWidget() override;

    void layoutGame();
};

#endif // SOLITAIREWIDGET_H
