#include "animateditem.h"
#include <QObject>
#include <QTimer>

AnimatedItem::AnimatedItem(QGraphicsItem *parent) : QGraphicsItem(parent) {}

AnimatedItem::~AnimatedItem()
{
    if (m_animation != nullptr) {
        m_animation->stop();
        delete m_animation;
    }
}

void AnimatedItem::setPosAnimated(const QPointF &newPos, int duration, int delay)
{
    if (m_animation != nullptr) {
        m_animation->stop();
        delete m_animation;
    }

    QPointF startPos = pos();
    m_animation = new QVariantAnimation();
    m_animation->setDuration(duration);
    m_animation->setStartValue(startPos);
    m_animation->setEndValue(newPos);
    m_animation->setEasingCurve(QEasingCurve::InOutQuad);

    QObject::connect(m_animation, &QVariantAnimation::valueChanged, [this](const QVariant &value) { setPos(value.toPointF()); });
    // connect(m_animation, &QVariantAnimation::finished, this, [this]() {
    //     delete m_animation;
    //     m_animation = nullptr;
    // });

    if (delay > 0) {
        QTimer::singleShot(delay, m_animation, [this]() { this->m_animation->start(); });
    } else {
        m_animation->start();
    }
}