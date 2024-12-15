#include "./bumpitem.h"
#include <hardware/bump/bump.hh>

namespace kiwi::widget::view2d {

    const QColor BumpItem::COLOR = Qt::black;
    const QColor BumpItem::HOVERED_COLOR = Qt::red;

    BumpItem::BumpItem(hardware::Bump* bump, QPointF position, QGraphicsItem *parent) :
        QGraphicsEllipseItem{parent},
        _bump{bump}
    {
        this->setRect(-RADIUS, -RADIUS, DIAMETER, DIAMETER);
        this->setPos(position);
        this->setBrush(COLOR); 
        this->setAcceptHoverEvents(true);
    }

    void BumpItem::hoverEnterEvent(QGraphicsSceneHoverEvent *) {
        this->setBrush(HOVERED_COLOR); 
        this->update();
    }

    void BumpItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *) {
        this->setBrush(COLOR); 
        this->update();
    }
    
    
}