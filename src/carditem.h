
#ifndef CARDITEM_H
#define CARDITEM_H

#include "animateditem.h"
#include "game.h"
#include <QGraphicsItem>
#include <QPixmap>

class SolitaireWidget;

class CardItem : public AnimatedItem
{
    SolitaireWidget *m_solitaireWidget;
    Card m_card;
    QPixmap m_pixmap;
    bool m_pixmapLoaded = false;
    bool m_highlighted = false;
    QPointF m_dragStartPos;
    bool m_dragging = false;
    QPointF m_dragStartMousePos; // just for distance calculation
    vector<QPointF> m_draggedCardStartPositions;

    void initPixmap();
    bool isValidMove(const vector<CardItem *> &draggedCardItems, Pile *sourcePile, Pile *destPile);

  public:
    CardItem(SolitaireWidget *solitaireWidget, const Card &card, QGraphicsItem *parent = nullptr);
    ~CardItem() override = default;

    const Card &card() const { return m_card; }
    void setCard(const Card &card);
    void setHighlighted(bool highlighted);
    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

  protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
};

#endif // CARDITEM_H
