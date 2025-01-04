#pragma once

#include "qcolor.h"
#include "qgraphicssceneevent.h"
#include <std/utility.hh>
#include <QGraphicsLineItem>
#include <QPen>
#include <QDebug>

namespace kiwi::widget::schematic {

    class PinItem;
    class NetPointItem;

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
        void updatePath();

    public:
        void setLine(const QPointF& begin, const QPointF& end);

    protected:
        auto boundingRect() const -> QRectF override;
        auto shape() const -> QPainterPath override;
        void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

        void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
        void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

        void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

    public:
        auto beginPoint() const -> NetPointItem* { return this->_beginPoint; }
        auto endPoint() const -> NetPointItem* { return this->_endPoint; }
        void setEndPoint(NetPointItem* point) { this->_endPoint = point; }

        auto isSyncNet() const -> bool;
        auto sync() const -> int;
        void setSync(int sync);
        
        auto isFloating() const -> bool;

    protected:
        QColor _color {COLOR};
        qreal _width {WIDTH};
        int _sync {};

        NetPointItem* _beginPoint {nullptr};
        NetPointItem* _endPoint {nullptr};

        QVector<QPointF> _points;
        QPainterPath _path;
        std::Option<QPointF> _tempPoint;
        QPointF _end;
    };

}