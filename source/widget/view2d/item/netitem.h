#pragma once

#include "qpoint.h"
#include <QColor>
#include <QGraphicsItem>
#include <QPainter>

namespace kiwi::widget::view2d {

    class NetItem : public QGraphicsLineItem {
    public:
        static const     QColor COLOR;
        static const     QColor HOVER_COLOR;
        static constexpr qreal  WIDTH = 1;
        static constexpr qreal  HOVER_WIDTH = 2;

    public:
        NetItem(const QPointF& begin, const QPointF& end);

    protected:
        void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
        void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
    };


}