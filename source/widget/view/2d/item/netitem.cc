#include "./netitem.h"
#include "./pinitem.h"
#include "qnamespace.h"
#include "qobject.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>

#include <QDebug>

namespace kiwi::widget {

    NetPointItem::NetPointItem(PinItem* connectedPin, QGraphicsItem* parent)
        : QGraphicsEllipseItem{parent}, 
        _connectedPin{connectedPin}
    {
        this->setRect(-RADIUS, -RADIUS, DIAMETER, DIAMETER);
        this->setBrush(Qt::blue);
        this->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemSendsGeometryChanges);
        this->setAcceptHoverEvents(true);

        this->linkToPin(connectedPin);
    }

    void NetPointItem::linkToPin(PinItem* pin) {
        if (pin != nullptr) {
            this->_connectedPin = pin;
            this->setPos(pin->scenePos());
            pin->setConnectedPoint(this); 
        }
    }

    auto NetPointItem::unlinkPin() -> PinItem* {
        auto pin = this->_connectedPin;
        this->_connectedPin = nullptr;
        pin->setConnectedPoint(nullptr);
        return pin;
    }

    QVariant NetPointItem::itemChange(GraphicsItemChange change, const QVariant& value) {
        if (change == GraphicsItemChange::ItemPositionChange && this->scene()) {
            emit positionChanged();
        }
        return QGraphicsEllipseItem::itemChange(change, value);
    }

    void NetPointItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
        setPen(QPen(Qt::red, 3)); // 高亮颜色
        QGraphicsEllipseItem::hoverEnterEvent(event);
    }

    void NetPointItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
        setPen(QPen(Qt::black, 2)); // 恢复颜色
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

            this->linkToPin(pin);
            this->setRect(-RADIUS, -RADIUS, DIAMETER, DIAMETER);
        }

        QGraphicsEllipseItem::mouseReleaseEvent(event);
    }

    ///////////////////////////////////////////////////////////////////////////

    NetItem::NetItem(PinItem* beginPin, PinItem* endPin) : 
        QGraphicsLineItem{},
        _beginPoint{new NetPointItem {beginPin, this}},
        _endPoint{new NetPointItem {endPin, this}}
    {
        QObject::connect(this->_beginPoint, &NetPointItem::positionChanged, this, &NetItem::updateLine);
        QObject::connect(this->_endPoint, &NetPointItem::positionChanged, this, &NetItem::updateLine);

        this->setAcceptHoverEvents(true);
        this->setFlags(QGraphicsItem::ItemIsSelectable);
        this->updateLine();
    }

    void NetItem::updateLine() {
        this->setLine(QLineF(this->_beginPoint->scenePos(), this->_endPoint->scenePos()));
    }
    
    void NetItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
        setPen(QPen(Qt::red, 3)); // 高亮颜色
        QGraphicsLineItem::hoverEnterEvent(event);
    }

    void NetItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
        setPen(QPen(Qt::black, 2)); // 恢复颜色
        QGraphicsLineItem::hoverLeaveEvent(event);
    }

}