#include "./topdieinstitem.h"
#include "./pinitem.h"
#include "../schematicscene.h"
#include "qchar.h"
#include "qcolor.h"
#include "qnamespace.h"
#include "qpoint.h"
#include "qregion.h"
#include "qvector.h"

#include <circuit/topdieinst/topdieinst.hh>
#include <circuit/topdie/topdie.hh>

#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>


namespace kiwi::widget::schematic {
    
    const QColor TopDieInstanceItem::COLOR = QColor::fromRgb(200, 200, 200, 100);

    TopDieInstanceItem::TopDieInstanceItem(circuit::TopDieInstance* topdieinst):
        GridItem{},
        _topdieinst{topdieinst}
    {
        this->setFlags(this->flags() | QGraphicsItem::ItemIsMovable);

        // Dive pinsize to four size:
        auto pinmap = this->_topdieinst->topdie()->pins_map();
        auto pinnames = QVector<QString>{};
        for (const auto& [name, _] : pinmap) {
            pinnames.push_back(QString::fromStdString(name));
        }

        this->_name = QString::fromStdString(topdieinst->name().data());

        /*
        
            +----+-------------------------+----+
            |    |        up pin           |    |
            +----+-------------------------+----+
            |    |                         |    |
            | l  |                         |    |
            | e  |                         |    |
            | f  |                         |    |
            | t  |                         |    |
            |    |          NAME           |    |
            | p  |                         |    |
            | i  |                         |    |
            | n  |                         |    |
            |    |                         |    |
            |    |                         |    |
            +----+-------------------------+----+
            |    |                         |    |
            +----+-------------------------+----+
        
        */
        auto pin_count = pinmap.size();

        auto pinsCount = pinmap.size();
        auto pinsPeSide = pinsCount / 4;
        auto remainder = pinsCount % 4;

        auto top_pins = pinsPeSide + (remainder > 0 ? 1 : 0);
        auto right_pins = pinsPeSide + (remainder > 1 ? 1 : 0);
        auto bottom_pins = pinsPeSide + (remainder > 2 ? 1 : 0);
        auto left_pins = pinsPeSide;
        
        auto pinAreaWidth  = (qMax(top_pins, bottom_pins) + 1) * PIN_INTERVAL;
        auto pinAreaHeight = (qMax(left_pins, right_pins) + 1) * PIN_INTERVAL;

        this->_width = pinAreaWidth + 2 * SPACE_LENGTH;
        this->_height = pinAreaHeight + 2 * SPACE_LENGTH;

        auto iter = pinnames.begin();
        this->createPins(top_pins, pinAreaWidth, SPACE_LENGTH, 0., iter, PinSide::Top); 
        this->createPins(right_pins, pinAreaHeight, this->_width, SPACE_LENGTH, iter, PinSide::Right); 
        this->createPins(bottom_pins, pinAreaWidth, SPACE_LENGTH, this->_height, iter, PinSide::Bottom); 
        this->createPins(left_pins, pinAreaHeight, 0, SPACE_LENGTH, iter, PinSide::Left);
    }

    QRectF TopDieInstanceItem::boundingRect() const {
        return QRectF{0., 0., this->_width,  this->_height};
    }

    void TopDieInstanceItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
        painter->setBrush(COLOR);
        painter->drawRect(this->boundingRect());

        auto length = this->_name.size() * NAME_CHAR_WIDTH;
        auto rect = QRectF {
            this->_width / 2. - length / 2.,
            this->_height / 2. - NAME_CHAR_HEIGHT / 2.,
            length,
            NAME_CHAR_HEIGHT
        };

        painter->drawText(rect, Qt::AlignCenter,this->_name);
    }

    void TopDieInstanceItem::createPins(int n, qreal side_length, qreal x_offset, qreal y_offset, QVector<QString>::iterator& iter, PinSide side) {
        if (n == 0) return;

        auto horizontal = side == PinSide::Bottom || side == PinSide::Top;

        double spacing = static_cast<double>(side_length) / (n + 1);

        for (int i = 0; i < n; ++i, ++iter) {
            double pos = (i + 1) * spacing;
            int x = horizontal ? static_cast<int>(pos + x_offset) : x_offset;
            int y = horizontal ? y_offset : static_cast<int>(pos + y_offset);

            auto *pin = new PinItem {*iter, QPointF(x, y), side, this};
            this->_pinitems.insert(*iter, pin);
        }
    }

}