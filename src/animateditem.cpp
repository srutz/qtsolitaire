#include "animateditem.h"
#include <QObject>
#include <QTimer>

AnimatedItem::AnimatedItem(QGraphicsItem *parent) : QGraphicsItem(parent) {}

AnimatedItem::~AnimatedItem()
{
    if (m_posAnimation != nullptr) {
        m_posAnimation->stop();
        delete m_posAnimation;
    }
    if (m_pulseAnimation != nullptr) {
        m_pulseAnimation->stop();
        delete m_pulseAnimation;
    }
}

void AnimatedItem::setPosAnimated(const QPointF &newPos, int duration, int delay)
{
    if (m_posAnimation != nullptr) {
        m_posAnimation->stop();
        delete m_posAnimation;
    }
    QPointF startPos = pos();
    m_posAnimation = new QVariantAnimation();
    m_posAnimation->setDuration(duration);
    m_posAnimation->setStartValue(startPos);
    m_posAnimation->setEndValue(newPos);
    m_posAnimation->setEasingCurve(QEasingCurve::InOutQuad);

    QObject::connect(
        m_posAnimation, &QVariantAnimation::valueChanged, [this](const QVariant &value) { setPos(value.toPointF()); });
    if (delay > 0) {
        QTimer::singleShot(delay, m_posAnimation, [this]() { this->m_posAnimation->start(); });
    } else {
        m_posAnimation->start();
    }
}

void AnimatedItem::pulse(int duration, int delay)
{
    if (m_pulseAnimation != nullptr) {
        m_pulseAnimation->stop();
        delete m_pulseAnimation;
    }

    m_pulseAnimation = new QVariantAnimation();
    m_pulseAnimation->setDuration(duration);
    m_pulseAnimation->setStartValue(1.0);
    m_pulseAnimation->setEndValue(1.0);
    m_pulseAnimation->setEasingCurve(QEasingCurve::InOutQuad);

    // Set keyframes for the pulse: 0% = 1.0, 50% = 1.2, 100% = 1.0
    m_pulseAnimation->setKeyValueAt(0.0, 1.0);
    m_pulseAnimation->setKeyValueAt(0.5, 1.2);
    m_pulseAnimation->setKeyValueAt(1.0, 1.0);

    QObject::connect(m_pulseAnimation, &QVariantAnimation::valueChanged, [this](const QVariant &value) {
        qreal scale = value.toReal();
        setScale(scale);
    });

    if (delay > 0) {
        QTimer::singleShot(delay, m_pulseAnimation, [this]() { this->m_pulseAnimation->start(); });
    } else {
        m_pulseAnimation->start();
    }
}