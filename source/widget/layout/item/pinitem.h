#pragma once

#include <QColor>
#include <QGraphicsItem>
#include <QPainter>
#include <QVector>

namespace kiwi::widget::layout {

    class NetItem;
    class ExternalPortItem;
    class SourcePortItem;
    class TopDieInstanceItem;

    class PinItem : public QGraphicsEllipseItem {
    public:
        static constexpr qreal RADIUS = 5.;
        static constexpr qreal DIAMETER = 2. * RADIUS;
        static const    QColor COLOR;
        static const    QColor HOVERED_COLOR;
        static constexpr int   Z_VALUE = 6;

        enum { Type = UserType + 4 };
        int type() const override { return Type; }

    public:
        PinItem(QPointF position, QGraphicsItem *parent = nullptr);

    protected:
        void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
        void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
        auto itemChange(GraphicsItemChange change, const QVariant& value) -> QVariant override;

    public:
        auto isExternalPortPin() const -> bool;
        auto isSourcePortPin() const -> bool;
        auto isTopDieInstancePin() const -> bool;

        auto parentExternalPort() const -> ExternalPortItem*;
        auto parentSourcePort() const -> SourcePortItem*;
        auto parentTopDieInsance() const -> TopDieInstanceItem*;

    public:
        void addNet(NetItem* net) 
        { this->_nets.push_back(net); }

        void removeNet(NetItem* net) 
        { this->_nets.removeOne(net); }

    private:
        QVector<NetItem*> _nets {};
    };


}