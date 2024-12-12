#include "./topdieinstitem.h"
#include "./pinitem.h"
#include "qchar.h"
#include "qnamespace.h"
#include "qpoint.h"
#include "qregion.h"

#include <circuit/topdie/topdieinst.hh>
#include <circuit/topdie/topdie.hh>

#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>


namespace kiwi::widget {
    
    TopDieInstanceItem::TopDieInstanceItem(circuit::TopDieInstance* topdieinst):
        _topdieinst{topdieinst} 
    {
        this->setFlags(ItemIsSelectable | ItemIsMovable);

        // Dive pinsize to four size:
        auto pinmap = this->_topdieinst->topdie()->pins_map();
        for (const auto& [name, _] : pinmap) {
            this->_pinNames.push_back(QString::fromStdString(name));
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
        auto pin_count = this->_pinNames.size();

        auto pinsCount = this->_pinNames.size();
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

        int index = 0;
        this->createPins(top_pins, pinAreaWidth, SPACE_LENGTH, 0., index, PinSide::Top); 
        this->createPins(right_pins, pinAreaHeight, this->_width, SPACE_LENGTH, index, PinSide::Right); 
        this->createPins(bottom_pins, pinAreaWidth, SPACE_LENGTH, this->_height, index, PinSide::Bottom); 
        this->createPins(left_pins, pinAreaHeight, 0, SPACE_LENGTH, index, PinSide::Left);
    }

    QRectF TopDieInstanceItem::boundingRect() const {
        return QRectF(0., 0., this->_width, this->_height);
    }

    QPainterPath TopDieInstanceItem::shape() const {
        QPainterPath path;
        path.addRect(this->boundingRect());
        return path;
    }

    void TopDieInstanceItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
        painter->setPen(Qt::black);
        painter->setBrush(Qt::lightGray);
        painter->drawRect(boundingRect());

        auto length = this->_name.size() * NAME_CHAR_WIDTH;
        auto rect = QRectF {
            this->_width / 2. - length / 2.,
            this->_height / 2. - NAME_CHAR_HEIGHT / 2.,
            length,
            NAME_CHAR_HEIGHT
        };

        painter->drawText(rect, Qt::AlignCenter,this->_name);
    }

    void TopDieInstanceItem::createPins(int n, qreal side_length, qreal x_offset, qreal y_offset, int &index, PinSide side) {
        if (n == 0) return;

        auto horizontal = side == PinSide::Bottom || side == PinSide::Top;

        double spacing = static_cast<double>(side_length) / (n + 1);

        for (int i = 0; i < n; ++i, ++index) {
            if (index >= static_cast<int>(this->_pinNames.size())) break;

            double pos = (i + 1) * spacing;
            int x = horizontal ? static_cast<int>(pos + x_offset) : x_offset;
            int y = horizontal ? y_offset : static_cast<int>(pos + y_offset);

            Pin *pin = new Pin{this->_pinNames[index], QPointF(x, y), side, this};
        }
    }

}