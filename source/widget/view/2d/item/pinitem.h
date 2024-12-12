#pragma once

#include <QColor>
#include <QGraphicsItem>
#include <QPainter>

namespace kiwi::widget {

    enum class PinSide {
        Top,
        Left,
        Right,
        Bottom,
    };

    class Pin : public QGraphicsItem {
    public:
        static constexpr qreal PIN_RADIUS = 5.;
        static constexpr qreal PIN_DIAMETER = 2 * PIN_RADIUS;
        static constexpr qreal NAME_INTERVAL = 10.;
        static constexpr qreal CHAR_WIDTH_ = 10.;
        static constexpr qreal CHAR_HEIGHT = 20.;
        static const    QColor COLOR;
        static const    QColor HOVERED_COLOR;
    
    public:
        Pin(const QString &name, QPointF position, PinSide side, QGraphicsItem *parent = nullptr);
        
        QRectF boundingRect() const override;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;

    protected:
        void hoverEnterEvent(QGraphicsSceneHoverEvent *) override;
        void hoverLeaveEvent(QGraphicsSceneHoverEvent *) override;

    private:
        QString _name;
        QPointF _position;
        PinSide _side;

        bool _hovered;
    };


}