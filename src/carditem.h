
#ifndef CARDITEM_H
#define CARDITEM_H

#include "game.h"
#include <QGraphicsItem>
#include <QPixmap>

class SolitaireWidget;

class CardItem : public QGraphicsItem
{
    SolitaireWidget *m_solitaireWidget;
    Card m_card;
    QPixmap m_pixmap;
    bool m_pixmapLoaded = false;
    QPointF m_dragStartPos;

    void initPixmap();

  public:
    CardItem(SolitaireWidget *solitaireWidget, const Card &card, QGraphicsItem *parent = nullptr);
    ~CardItem() override = default;

    const Card &card() const { return m_card; }
    void setCard(const Card &card)
    {
        m_card = card;
        update();
    }

    virtual QRectF boundingRect() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

  protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
};

#endif // CARDITEM_H
