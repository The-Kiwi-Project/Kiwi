#pragma once

#include <QGraphicsPathItem>
#include <QPen>

namespace kiwi::widget {

    class NetItem : public QGraphicsPathItem {
    public:
        NetItem(const QPainterPath& path) : QGraphicsPathItem(path) {
            this->setAcceptHoverEvents(true);
        }

    protected:
        void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override {
            setPen(QPen(Qt::red, 3)); // 高亮颜色
            QGraphicsPathItem::hoverEnterEvent(event);
        }

        void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override {
            setPen(QPen(Qt::black, 2)); // 恢复颜色
            QGraphicsPathItem::hoverLeaveEvent(event);
        }
    };

}