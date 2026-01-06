
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
    vector<CardItem *> m_draggedCardItems;
    PileItem *m_highlightedPile = nullptr;
    CardItem *m_highlightedCard = nullptr;

    void initPixmap();
    CardItem *findCardItem(const Card &card) const;
    PileItem *findPileItem(PileType pileType, int index) const;

  public:
    explicit SolitaireWidget(QWidget *parent = nullptr);
    ~SolitaireWidget() override;

    const QPixmap &backPixmap() const { return s_backPixmap; }
    Game &game() { return m_game; }
    void layoutGame(bool delayed = false);

    vector<CardItem *> &draggedCardItems() { return m_draggedCardItems; }
    void clearDraggedCards() { m_draggedCardItems.clear(); }
    void setHighlightedPile(PileItem *pile);
    PileItem *findPileItemAt(const QPointF &scenePos);
    Pile *getPileForPileItem(PileItem *pileItem);
};

#endif // SOLITAIREWIDGET_H
