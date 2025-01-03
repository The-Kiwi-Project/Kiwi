#include "./exportitem.h"
#include "qnamespace.h"
#include "qobject.h"
#include "qpoint.h"
#include "widget/schematic/item/pinitem.h"

namespace kiwi::widget::schematic {
    
    const QColor ExPortItem::COLOR = Qt::lightGray;

    ExPortItem::ExPortItem(const QString name, SchematicScene* scene) : 
        QGraphicsItem{},
        _pinietm{},
        _width{PIN_SIDE_INTERVAL + PinItem::NAME_INTERVAL + name.size() * PinItem::CHAR_WIDTH_ + PIN_SIDE_INTERVAL}
    {
        this->_pinietm = new PinItem{name, QPointF{0, 0}, PinSide::Left, scene, this};
        this->setFlags(ItemIsMovable);
    }

    auto ExPortItem::boundingRect() const -> QRectF {
        return QRectF {-PIN_SIDE_INTERVAL, -HEIGHT / 2., this->_width, HEIGHT};
    }

    void ExPortItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
        painter->setBrush(COLOR);
        painter->drawRect(this->boundingRect());
    }

}