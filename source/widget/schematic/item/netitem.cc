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

    NetPointItem::NetPointItem(PinItem* connectedPin, QGraphicsItem* parent): 
        QGraphicsEllipseItem{parent}, 
        _connectedPin{connectedPin}
    {
        this->setRect(-RADIUS, -RADIUS, DIAMETER, DIAMETER);
        this->setBrush(COLOR);
        this->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemSendsGeometryChanges);
        this->setAcceptHoverEvents(true);

        this->linkToPin(connectedPin);
        this->setZValue(10);
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
        if (change == GraphicsItemChange::ItemPositionChange && this->scene()) {
            this->_netitem->updateLine();
        }
        return QGraphicsEllipseItem::itemChange(change, value);
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

    ///////////////////////////////////////////////////////////////////////////

    const QColor NetItem::COLOR = Qt::black;
    const QColor NetItem::HOVER_COLOR = Qt::red;

    NetItem::NetItem(NetPointItem* beginPoint, NetPointItem* endPoint):
        QGraphicsLineItem{},
        _beginPoint{beginPoint},
        _endPoint{endPoint}
    {
        this->_beginPoint->setNetItem(this);
        this->_endPoint->setNetItem(this);

        this->setAcceptHoverEvents(true);
        this->updateLine();
    }

    NetItem::NetItem(NetPointItem* beginPoint) :
        QGraphicsLineItem{},
        _beginPoint{beginPoint}
    {
        this->_beginPoint->setNetItem(this);
        this->setAcceptHoverEvents(true);
        this->setLine(QLineF(this->_beginPoint->scenePos(), this->_beginPoint->scenePos()));
    }

    void NetItem::updateEndPoint(const QPointF& point) {
        this->setLine(QLineF(this->_beginPoint->scenePos(), point));
    }

    void NetItem::updateLine() {
        if (this->_beginPoint == nullptr || this->_endPoint == nullptr) {
            return;
        }
        this->setLine(QLineF(this->_beginPoint->scenePos(), this->_endPoint->scenePos()));
    }
    
    void NetItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
        this->setPen(QPen{HOVER_COLOR, HOVER_WIDTH});
        QGraphicsLineItem::hoverEnterEvent(event);
    }

    void NetItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
        this->setPen(QPen{COLOR, WIDTH});
        QGraphicsLineItem::hoverLeaveEvent(event);
    }

}