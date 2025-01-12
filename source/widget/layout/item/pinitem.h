#pragma once

#include <QColor>
#include <QGraphicsItem>
#include <QPainter>
#include <QVector>

namespace kiwi::widget::layout {

    class NetItem;
    
    class PinItem : public QGraphicsEllipseItem {
    public:
        static constexpr qreal RADIUS = 5.;
        static constexpr qreal DIAMETER = 2. * RADIUS;
        static const    QColor COLOR;
        static const    QColor HOVERED_COLOR;
        static constexpr int   Z_VALUE = 6;

    public:
        PinItem(QPointF position, QGraphicsItem *parent = nullptr);

    protected:
        void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
        void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
        auto itemChange(GraphicsItemChange change, const QVariant& value) -> QVariant override;

    public:
        void addNet(NetItem* net) 
        { this->_nets.push_back(net); }

    private:
        QVector<NetItem*> _nets {};
    };


}