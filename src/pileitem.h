

#ifndef PILEITEM_H
#define PILEITEM_H

#include "game.h"
#include <QGraphicsItem>

class SolitaireWidget;

class PileItem : public QGraphicsItem
{
    Pile m_pile;
    bool m_highlighted = false;
    SolitaireWidget *m_solitaireWidget = nullptr;

  public:
    PileItem(const Pile &pile, QGraphicsItem *parent = nullptr);
    PileItem(const Pile &pile, SolitaireWidget *solitaireWidget, QGraphicsItem *parent = nullptr);
    ~PileItem() override = default;

    const Pile &pile() const { return m_pile; }
    void setHighlighted(bool highlighted)
    {
        m_highlighted = highlighted;
        update();
    }
    bool isHighlighted() const { return m_highlighted; }

    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

  protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
};

#endif // PILEITEM_H
