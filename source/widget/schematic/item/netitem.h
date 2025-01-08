#pragma once

#include "qcolor.h"
#include "qglobal.h"
#include "qgraphicssceneevent.h"
#include <std/utility.hh>
#include <QGraphicsLineItem>
#include <QPen>
#include <QDebug>

namespace kiwi::circuit {
    class Connection;
}

namespace kiwi::widget::schematic {

    class PinItem;
    class NetPointItem;

    class NetItem : public QGraphicsItem {
    public:
        static const     QColor DEFAULT_COLOR;
        static constexpr qreal  DEFAULT_WIDTH = 2;

        static const     QColor HOVER_COLOR;

        enum { Type = UserType + 3 };
        int type() const override { return Type; }
        
    public:
        NetItem(circuit::Connection* connection, NetPointItem* beginPoint, NetPointItem* endPoint);
        NetItem(NetPointItem* beginPoint);

    public:
        void updateLine();
        void updatePositionFrom(NetPointItem* pointItem, const QPointF& pos);
        void updateEndPoint(const QPointF& point);
        void addPoint(const QPointF& point);

    private:
        void updateBeginPosition(const QPointF& pos);
        void updateEndPosition(const QPointF& pos);
        void updatePath();

    public:
        void setLine(const QPointF& begin, const QPointF& end);

    protected:
        auto boundingRect() const -> QRectF override;
        auto shape() const -> QPainterPath override;
        void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

        void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
        void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

    public:
        auto beginPoint() const -> NetPointItem* { return this->_beginPoint; }
        auto endPoint() const -> NetPointItem* { return this->_endPoint; }
        void setEndPoint(NetPointItem* point) { this->_endPoint = point; }
        void setConnection(circuit::Connection* connection) { this->_connection = connection; }
        auto connection() const -> circuit::Connection* { return this->_connection; }

        auto color() const -> const QColor& { return this->_color; }
        auto setColor(const QColor& color) { this->_color = color; this->_paintColor = color; }

        auto width() const -> qreal { return this->_width; }
        auto setWidth(qreal width) { this->_width = width; this->_paintWidth = width; }

        void resetPaint();
        auto isFloating() const -> bool;

    protected:
        circuit::Connection* _connection {nullptr};

        NetPointItem* _beginPoint {nullptr};
        NetPointItem* _endPoint {nullptr};

        QColor _paintColor {DEFAULT_COLOR};
        qreal  _paintWidth {DEFAULT_WIDTH};
        
        QColor _color {DEFAULT_COLOR};
        qreal  _width {DEFAULT_WIDTH};

        QVector<QPointF> _points;
        QPainterPath _path;
        std::Option<QPointF> _tempPoint;
        QPointF _end;
    };

}