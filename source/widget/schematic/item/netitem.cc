#include "./netitem.h"
#include "./pinitem.h"
#include "debug/debug.hh"
#include "qdebug.h"
#include "qglobal.h"
#include "qnamespace.h"
#include "qobject.h"
#include "qpoint.h"

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
        this->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemSendsScenePositionChanges);
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
        this->_netitem->updatePositionFrom(this);
    }

    QVariant NetPointItem::itemChange(GraphicsItemChange change, const QVariant& value) {
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
        QGraphicsItem{},
        _beginPoint{beginPoint},
        _endPoint{endPoint}
    {
        this->_beginPoint->setNetItem(this);
        this->_endPoint->setNetItem(this);
        this->setAcceptHoverEvents(true);
        this->updateLine();
    }

    NetItem::NetItem(NetPointItem* beginPoint) :
        QGraphicsItem{},
        _beginPoint{beginPoint}
    {
        this->_beginPoint->setNetItem(this);
        this->setAcceptHoverEvents(true);
        this->setLine(this->_beginPoint->scenePos(), this->_beginPoint->scenePos());
    }

    void NetItem::updateEndPoint(const QPointF& point) {
        if (this->_tempPoint.has_value()) {
            auto tempPoint = *this->_tempPoint;
            auto lastPoint = this->_points.back();
            auto isHoverLine = (lastPoint.y() == tempPoint.y());

            if (tempPoint == point) {
                this->_tempPoint.reset();
            }
            else if (tempPoint.x() != point.x() && tempPoint.y() != point.y()) {
                if (isHoverLine) {
                    this->_tempPoint = QPointF{point.x(), tempPoint.y()};
                } else {
                    this->_tempPoint = QPointF{tempPoint.x(), point.y()};
                }
            }
        }
        else {
            auto lastPoint = this->_points.back();
            if (lastPoint == point) {
                
            }
            else if (lastPoint.x() == point.x() || lastPoint.y() == point.y()) {

            } else {
                if (lastPoint != this->_end) {
                    this->_tempPoint = this->_end;
                }
            }
        }

        this->_end = point;
        this->updatePath();
    }

    void NetItem::addPoint(const QPointF& point) {
        if (this->_tempPoint.has_value()) {
            this->_points.push_back(*this->_tempPoint);
            this->_tempPoint.reset();
        }
        this->_points.push_back(point);
    }

    void NetItem::updateLine() {
        if (this->_beginPoint == nullptr || this->_endPoint == nullptr) {
            return;
        }
        this->setLine(this->_beginPoint->scenePos(), this->_endPoint->scenePos());
    }

    void NetItem::updatePositionFrom(NetPointItem* pointItem) {
        if (pointItem == this->_beginPoint) {
            this->updateBeginPosition(pointItem->scenePos());
        } else if (pointItem == this->_endPoint) {
            this->updateEndPosition(pointItem->scenePos());
        } else {
            debug::unreachable();
        }
    }

    void NetItem::updateBeginPosition(const QPointF& pos) {
        this->prepareGeometryChange();

        this->update();
    }

    void NetItem::updateEndPosition(const QPointF& pos) {
        this->prepareGeometryChange();

        if (this->_points.size() == 2) {
            auto beginPos = this->_points[0];
            auto endPos = this->_points[1];
            
            if (beginPos.x() == endPos.x()) {
                if (pos == endPos) {

                }
                else if (pos.x() == endPos.x()) {
                    this->_points[1] = pos;
                }
                else if (pos.y() == endPos.y()) {
                    this->_points.push_back(pos);
                }
            }
            else if (beginPos.y() == endPos.y()) {
                if (pos == endPos) {

                }
                else if (pos.x() == endPos.x()) {
                    this->_points.push_back(pos);
                }
                else if (pos.y() == endPos.y()) {
                    this->_points[1] = pos;
                }
            } 
            else {
                debug::exception("The netitem has unparall points");
            }
        }
        else {
            auto endIter = this->_points.rbegin();
            auto lastIter = endIter + 1;

            auto endPos = *endIter;
            auto lastPos = *lastIter;
            auto isHoverLine = endPos.y() == lastPos.y();

            if (endPos == pos) {

            } 
            else if (isHoverLine) {
                // if (pos == lastPos) {
                //     this->_points.pop_back();
                // }
                // else 
                if (pos.y() == endPos.y()) {
                    *endIter = pos;
                } 
                else {
                    *endIter = pos;
                    lastIter->setY(pos.y());
                }
            }
            else {
                // if (pos == lastPos) {
                //     this->_points.pop_back();
                // }
                // else 
                if (pos.x() == endPos.x()) {
                    *endIter = pos;
                } 
                else {
                    *endIter = pos;
                    lastIter->setX(pos.x());
                }
            }
        }

        this->_path = QPainterPath{};
        if (!this->_points.isEmpty()) {
            this->_path.moveTo(this->_points[0]);
            for (int i = 1; i < this->_points.size(); ++i) {
                this->_path.lineTo(this->_points[i]);
            }
        }

        this->update();
    }

    auto NetItem::boundingRect() const -> QRectF {
        return this->_path.boundingRect().adjusted(-2, -2, 2, 2);
    }

    void NetItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
        QPen pen(Qt::black, 2);
        painter->setPen(pen);
        painter->drawPath(this->_path);
    }

    void NetItem::setLine(const QPointF& begin, const QPointF& end) {
        this->prepareGeometryChange();

        this->_path.clear();
        this->_path.moveTo(begin);
        if (begin.x() == end.x() || begin.y() == end.y()) {
            this->_path.lineTo(end); // 单一方向
        } else {
            QPointF mid(end.x(), begin.y());
            this->_path.lineTo(mid);  // 水平转折点
            this->_path.lineTo(end);
        }

        this->_points.push_back(begin);
        this->_end = end;

        this->update();
    }
    
    void NetItem::updatePath() {
        this->prepareGeometryChange();

        this->_path = QPainterPath{};
        if (!this->_points.isEmpty()) {
            this->_path.moveTo(this->_points[0]);
            for (int i = 1; i < this->_points.size(); ++i) {
                this->_path.lineTo(this->_points[i]);
            }
        }
        if (this->_tempPoint.has_value()) {
            this->_path.lineTo(*this->_tempPoint);
        }
        this->_path.lineTo(this->_end);

        this->update();
    }

    void NetItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
        // this->setPen(QPen{HOVER_COLOR, HOVER_WIDTH});
        QGraphicsItem::hoverEnterEvent(event);
    }

    void NetItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
        // this->setPen(QPen{COLOR, WIDTH});
        QGraphicsItem::hoverLeaveEvent(event);
    }

}