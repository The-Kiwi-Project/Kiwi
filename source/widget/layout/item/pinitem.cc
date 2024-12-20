#include "./pinitem.h"
#include "./netitem.h"
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

        this->setFlags(ItemSendsScenePositionChanges | ItemIsSelectable);
    }

    void PinItem::hoverEnterEvent(QGraphicsSceneHoverEvent * e) {
        this->setBrush(HOVERED_COLOR); 
        QGraphicsEllipseItem::hoverEnterEvent(e);
    }

    void PinItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *e) {
        this->setBrush(COLOR); 
        QGraphicsEllipseItem::hoverLeaveEvent(e);
    }
    
    auto PinItem::itemChange(GraphicsItemChange change, const QVariant& value) -> QVariant {
        if (change == QGraphicsItem::ItemScenePositionHasChanged) {
            for (auto net : this->_nets) {
                net->updateLine();
            }
        }
        return QGraphicsItem::itemChange(change, value);
    }
    
}