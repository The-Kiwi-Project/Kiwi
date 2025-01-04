#pragma once

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