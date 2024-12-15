#include "./pinitem.h"
#include <hardware/bump/bump.hh>

namespace kiwi::widget::layout {

    const QColor PinItem::COLOR = Qt::black;
    const QColor PinItem::HOVERED_COLOR = Qt::red;

    PinItem::PinItem(hardware::Bump* bump, QPointF position, QGraphicsItem *parent) :
        QGraphicsEllipseItem{parent},
        _bump{bump}
    {
        this->setRect(-RADIUS, -RADIUS, DIAMETER, DIAMETER);
        this->setPos(position);
        this->setBrush(COLOR); 
        this->setAcceptHoverEvents(true);
    }

    void PinItem::hoverEnterEvent(QGraphicsSceneHoverEvent *) {
        this->setBrush(HOVERED_COLOR); 
        this->update();
    }

    void PinItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *) {
        this->setBrush(COLOR); 
        this->update();
    }
    
    
}