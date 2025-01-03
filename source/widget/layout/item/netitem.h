#pragma once

#include <QColor>
#include <QGraphicsItem>
#include <QPainter>

namespace kiwi::widget::layout {

    class PinItem;

    class NetItem : public QGraphicsLineItem {
    public:
        static const     QColor COLOR;
        static const     QColor HOVER_COLOR;
        static constexpr qreal  WIDTH = 2;
        static constexpr qreal  HOVER_WIDTH = 3;

    public:
        NetItem(PinItem* beginPin, PinItem* endPin, QGraphicsLineItem *parent = nullptr);

    protected:
        void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
        void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

    public:
        void updateLine();

    private:
        PinItem* _beginPin;
        PinItem* _endPin;
    };


}