#pragma once

#include <QGraphicsLineItem>
#include <QPen>
#include <QDebug>

namespace kiwi::widget::schematic {

    class PinItem;
    class NetItem;
    
    class NetPointItem : public QGraphicsEllipseItem {
    public:
        static constexpr qreal RADIUS = 5.;
        static constexpr qreal DIAMETER = 2. * RADIUS;

        static constexpr qreal MOVING_RADIUS = 8.;
        static constexpr qreal MOVING_DIAMETER = 2. * MOVING_RADIUS;
        static const     QColor COLOR;
        static const     QColor HOVER_COLOR;
    
    public:
        NetPointItem(PinItem* connectedPin, QGraphicsItem* parent = nullptr);

    public:
        void linkToPin(PinItem* pin);
        auto unlinkPin() -> PinItem*;

        void setNetItem(NetItem* netitem) 
        { this->_netitem = netitem; }

        void updatePos();

    protected:
        QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

    protected:
        void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
        void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
        void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
        void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
        void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
        void mouseDoubleClickEvent(QGraphicsSceneMouseEvent*) override;

    public:
        auto connectedPin() const -> PinItem* { return this->_connectedPin; }

    private:
        bool _dragging {false};
        PinItem* _connectedPin {nullptr};
        NetItem* _netitem {nullptr};
    };

}