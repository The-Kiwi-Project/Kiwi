#include "./netitem.h"
#include "./pinitem.h"
#include <cassert>
#include <cstdlib>
#include <debug/debug.hh>

namespace kiwi::widget::layout {

    const QColor NetItem::COLOR = Qt::black;
    const QColor NetItem::HOVER_COLOR = Qt::red;

    NetItem::NetItem(PinItem* beginPin, PinItem* endPin, QGraphicsLineItem *parent) :
        QGraphicsLineItem{parent},
        _beginPin{beginPin},
        _endPin{endPin}
    {
        this->setZValue(Z_VALUE);

        if (this->_beginPin == nullptr || this->_endPin == nullptr) {
            debug::exception("Nullptr in begin pin or end pin");
        }    
        this->_beginPin->addNet(this);
        this->_endPin->addNet(this);
        this->updateLine();
    }

    void NetItem::updateLine() {
        this->setLine(QLineF{this->_beginPin->scenePos(), this->_endPin->scenePos()});
    }

    void NetItem::moveToBeginPin(PinItem* pin) {
        assert(this->_beginPin != nullptr);
        assert(pin != nullptr);
        this->_beginPin->removeNet(this);
        this->_beginPin = pin;
        pin->addNet(this);
    }

    void NetItem::moveToEndPin(PinItem* pin) {
        assert(this->_endPin != nullptr);
        assert(pin != nullptr);
        this->_endPin->removeNet(this);
        this->_endPin = pin;
        pin->addNet(this);
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