#pragma once

#include "qobject.h"
#include "qpoint.h"
#include <QGraphicsLineItem>
#include <QPen>
#include <QDebug>

namespace kiwi::widget {

    class PinItem;

    class NetPointItem : public QObject, public QGraphicsEllipseItem {
        Q_OBJECT

    public:
        static constexpr qreal RADIUS = 5.;
        static constexpr qreal DIAMETER = 2. * RADIUS;

        static constexpr qreal MOVING_RADIUS = 8.;
        static constexpr qreal MOVING_DIAMETER = 2. * MOVING_RADIUS;
    

    public:
        NetPointItem(PinItem* connectedPin, QGraphicsItem* parent = nullptr);

        void linkToPin(PinItem* pin);
        auto unlinkPin() -> PinItem*;

    protected:
        QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

    protected:
        void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
        void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
        void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
        void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
        void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

    signals:
        void positionChanged();

    private:
        bool _dragging {false};
        PinItem* _connectedPin {nullptr};
    };

    class NetItem : public QObject, public QGraphicsLineItem {
        Q_OBJECT

    public:
        NetItem(PinItem* beginPin, PinItem* endPin);

    public:
        void updateLine();

    protected:
        void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
        void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

    public:
        auto beginPoint() const -> NetPointItem* { return this->_beginPoint; }
        auto endPoint() const -> NetPointItem* { return this->_endPoint; }

    protected:
        NetPointItem* const _beginPoint;
        NetPointItem* const _endPoint;

        QPointF _dragStartOffset;
    };

}