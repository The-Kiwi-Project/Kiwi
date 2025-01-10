#include "./netpointitem.h"
#include "./netitem.h"
#include "./pinitem.h"
#include "qnamespace.h"
#include "qobject.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>

#include <QDebug>

namespace kiwi::widget::schematic {

    const QColor NetPointItem::COLOR = Qt::blue;
    const QColor NetPointItem::HOVER_COLOR = Qt::red;

    NetPointItem::NetPointItem(PinItem* connectedPin): 
        QGraphicsEllipseItem{nullptr}, 
        _connectedPin{connectedPin}
    {
        this->setRect(-RADIUS, -RADIUS, DIAMETER, DIAMETER);
        this->setBrush(COLOR);
        this->setFlags(this->flags() | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemSendsScenePositionChanges);
        this->setAcceptHoverEvents(true);

        this->linkToPin(connectedPin);
        this->setZValue(1);
    }
 
    void NetPointItem::linkToPin(PinItem* pin) {
        if (pin != nullptr) {
            this->_connectedPin = pin;
            this->setPos(pin->scenePos());
            pin->addConnectedPoint(this);        
        }
    }

    auto NetPointItem::unlinkPin() -> PinItem* {
        auto pin = this->_connectedPin;
        this->_connectedPin = nullptr;
        pin->removeConnectedPoint(this);
        return pin;
    }

    void NetPointItem::updatePos() {
        this->setPos(this->_connectedPin->scenePos());
    }

    QVariant NetPointItem::itemChange(GraphicsItemChange change, const QVariant& value) {
        auto v = QGraphicsEllipseItem::itemChange(change, value);
        if (change == GraphicsItemChange::ItemPositionChange && this->_netitem != nullptr) {
            if (this->_netitem->isFloating()) {
                return v;
            }
            this->_netitem->updatePositionFrom(this, v.toPointF());
        }
        return v;
    }

    void NetPointItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
        setPen(QPen(HOVER_COLOR, 3)); 
        QGraphicsEllipseItem::hoverEnterEvent(event);
    }

    void NetPointItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
        setPen(QPen(COLOR, 2));
        QGraphicsEllipseItem::hoverLeaveEvent(event);
    }

    void NetPointItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
        if (event->button() == Qt::LeftButton) {
            this->_dragging = true;
            this->setRect(-MOVING_RADIUS, -MOVING_RADIUS, MOVING_DIAMETER, MOVING_DIAMETER);
        }
        QGraphicsEllipseItem::mousePressEvent(event);
    }

    void NetPointItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
        QGraphicsEllipseItem::mouseMoveEvent(event);
    }

    void NetPointItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
        if (this->_dragging) {
            auto pin = this->_connectedPin;
            auto items = this->scene()->items(event->scenePos());
            for (auto* item : items) {
                if (item->type() == PinItem::Type) {
                    pin = dynamic_cast<PinItem*>(item);
                    break;
                }
            }

            if (this->_connectedPin != nullptr) {
                this->unlinkPin();
            }
            this->linkToPin(pin);
            this->setRect(-RADIUS, -RADIUS, DIAMETER, DIAMETER);

            this->_dragging = false;
        }

        QGraphicsEllipseItem::mouseReleaseEvent(event);
    }

    void NetPointItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) {
        event->ignore(); 
    }

}