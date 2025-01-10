#include "./sourceportitem.h"
#include <circuit/export/export.hh>
#include "qobject.h"
#include "widget/schematic/item/griditem.h"
#include "widget/schematic/item/pinitem.h"

namespace kiwi::widget::schematic {
    
    const QColor SourcePortItem::VDD_COLOR = QColor::fromRgb(255, 100, 100, 100);
    const QColor SourcePortItem::GND_COLOR = QColor::fromRgb(100, 255, 100, 100);

    SourcePortItem::SourcePortItem(const QString& name, SourcePortType type) : 
        GridItem{},
        _pin{nullptr},
        _type{type},
        _width{0}
    {
        this->_pin = new PinItem{name, QPointF{0, 0}, PinSide::Left, this};
        this->_width = GridItem::snapToGrid(PIN_SIDE_INTERVAL + PinItem::NAME_INTERVAL + name.size() * PinItem::CHAR_WIDTH_ + PIN_SIDE_INTERVAL);

        this->setFlags(this->flags() | QGraphicsItem::ItemIsMovable);
    }

    auto SourcePortItem::boundingRect() const -> QRectF {
        return QRectF {-PIN_SIDE_INTERVAL, -HEIGHT / 2., this->_width, HEIGHT};
    }

    void SourcePortItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
        painter->setBrush(this->_type == SourcePortType::VDD ? VDD_COLOR : GND_COLOR);
        painter->drawRect(this->boundingRect());
    }

}