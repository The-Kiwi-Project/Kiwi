#include "./sourceportitem.h"
#include "./pinitem.h"

namespace kiwi::widget::layout {

    const QColor SourcePortItem::VDD_COLOR = QColor::fromRgb(255, 100, 100, 100);
    const QColor SourcePortItem::GND_COLOR = QColor::fromRgb(100, 255, 100, 100); 

    SourcePortItem::SourcePortItem(SoucePortType type) :
        _type{type},
        _pin{nullptr}
    {
        this->setZValue(Z_VALUE);
        this->_pin = new PinItem {QPointF{0.0, 0.0}, this};
    }

    auto SourcePortItem::boundingRect() const -> QRectF {
        return QRectF(-WIDTH / 2., -WIDTH / 2., WIDTH, WIDTH);
    }

    void SourcePortItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
        painter->setBrush(this->_type == SoucePortType::VDD ? VDD_COLOR : GND_COLOR);
        painter->drawRect(-WIDTH / 2., -WIDTH / 2., WIDTH, WIDTH);
    }

}