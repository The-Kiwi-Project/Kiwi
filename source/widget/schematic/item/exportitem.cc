#include "./exportitem.h"
#include <circuit/export/export.hh>
#include "qobject.h"
#include "widget/schematic/item/griditem.h"
#include "widget/schematic/item/pinitem.h"

namespace kiwi::widget::schematic {
    
    const QColor ExPortItem::COLOR = QColor::fromRgb(200, 200, 200, 100);

    ExPortItem::ExPortItem(circuit::ExternalPort* eport) : 
        GridItem{},
        _eport{eport},
        _pinietm{nullptr},
        _width{0}
    {
        auto name = QString::fromStdString(eport->name());
        this->_pinietm = new PinItem{name, QPointF{0, 0}, PinSide::Left, this};
        this->_width = GridItem::snapToGrid(PIN_SIDE_INTERVAL + PinItem::NAME_INTERVAL + name.size() * PinItem::CHAR_WIDTH_ + PIN_SIDE_INTERVAL);

        this->setFlags(this->flags() | QGraphicsItem::ItemIsMovable);
    }

    auto ExPortItem::boundingRect() const -> QRectF {
        return QRectF {-PIN_SIDE_INTERVAL, -HEIGHT / 2., this->_width, HEIGHT};
    }

    void ExPortItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
        painter->setBrush(COLOR);
        painter->drawRect(this->boundingRect());
    }

}