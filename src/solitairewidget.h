
#ifndef SOLITAIREWIDGET_H
#define SOLITAIREWIDGET_H

#include "carditem.h"
#include "game.h"
#include "pileitem.h"
#include <QGraphicsScene>
#include <QPixmap>
#include <QWidget>

class SolitaireWidget : public QWidget
{
    Q_OBJECT

    QGraphicsScene *m_scene = nullptr;
    Game m_game;
    vector<PileItem *> m_pileItems;
    vector<CardItem *> m_cardItems;
    bool m_pixmapLoaded = false;
    QPixmap s_backPixmap;

    void initPixmap();
    CardItem *findCardItem(const Card &card) const;
    PileItem *findPileItem(PileType pileType, int index) const;

  public:
    explicit SolitaireWidget(QWidget *parent = nullptr);
    ~SolitaireWidget() override;

    const QPixmap &backPixmap() const { return s_backPixmap; }
    Game &game() { return m_game; }
    void layoutGame();
};

#endif // SOLITAIREWIDGET_H
