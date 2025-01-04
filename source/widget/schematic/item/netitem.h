#pragma once

#include "qcolor.h"
#include "qobject.h"
#include "qpoint.h"
#include "std/utility.hh"
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

    private:
        bool _dragging {false};
        PinItem* _connectedPin {nullptr};
        NetItem* _netitem {nullptr};
    };

    class NetItem : public QGraphicsItem {
    public:
        static const     QColor COLOR;
        static const     QColor HOVER_COLOR;
        static constexpr qreal  WIDTH = 2;
        static constexpr qreal  HOVER_WIDTH = 3;
        
    public:
        NetItem(NetPointItem* beginPoint, NetPointItem* endPoint);
        NetItem(NetPointItem* beginPoint);

    public:
        void updateLine();
        void updatePositionFrom(NetPointItem* pointItem, const QPointF& pos);
        void updateEndPoint(const QPointF& point);
        void addPoint(const QPointF& point);

    private:
        void updateBeginPosition(const QPointF& pos);
        void updateEndPosition(const QPointF& pos);

    public:
        void setLine(const QPointF& begin, const QPointF& end);

    protected:
        auto boundingRect() const -> QRectF override;
        void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

        void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
        void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

    public:
        auto beginPoint() const -> NetPointItem* { return this->_beginPoint; }
        auto endPoint() const -> NetPointItem* { return this->_endPoint; }

        auto isFloating() const -> bool {
            return this->_beginPoint == nullptr || this->_endPoint == nullptr;
        }

        void setEndPoint(NetPointItem* point) { this->_endPoint = point; }

    private:
        void updatePath();

    protected:
        NetPointItem* _beginPoint {nullptr};
        NetPointItem* _endPoint {nullptr};

        QVector<QPointF> _points;
        QPainterPath _path;
        std::Option<QPointF> _tempPoint;
        QPointF _end;
    };

}