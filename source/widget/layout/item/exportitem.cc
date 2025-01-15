#include "./exportitem.h"
#include "./pinitem.h"

namespace kiwi::widget::layout {

    const QColor ExternalPortItem::COLOR = Qt::blue;

    ExternalPortItem::ExternalPortItem(circuit::ExternalPort* eport) :
        _eport{eport},
        _pin{nullptr}
    {
        this->setZValue(Z_VALUE);
        this->_pin = new PinItem {QPointF{0.0, 0.0}, this};
    }

    auto ExternalPortItem::boundingRect() const -> QRectF {
        return QRectF(-WIDTH / 2., -WIDTH / 2., WIDTH, WIDTH);
    }

    void ExternalPortItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
        painter->setBrush(COLOR);
        painter->drawRect(-WIDTH / 2., -WIDTH / 2., WIDTH, WIDTH);
    }

}