#include "./netitem.h"
#include <debug/debug.hh>

namespace kiwi::widget::view2d {

    const QColor NetItem::COLOR = Qt::black;
    const QColor NetItem::HOVER_COLOR = Qt::red;

    NetItem::NetItem(const QPointF& begin, const QPointF& end) :
        QGraphicsLineItem{}
    {
        this->setAcceptHoverEvents(true);
        this->setLine(QLineF{begin, end});
        this->setPen(QPen(COLOR, WIDTH));
    }

    void NetItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
        this->setPen(QPen{HOVER_COLOR, HOVER_WIDTH});
        QGraphicsLineItem::hoverEnterEvent(event);
    }

    void NetItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
        this->setPen(QPen(COLOR, WIDTH));
        QGraphicsLineItem::hoverLeaveEvent(event);
    }

}