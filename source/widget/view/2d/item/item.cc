#include "./item.hh"
#include "qcolor.h"
#include "qpen.h"
#include "qpoint.h"

namespace kiwi::widget {

    Item::Item(const QPoint& center, int width, int height) :
        _center{center},
        _width{width},
        _height{height}
    {
    }

    auto Item::boundingRect() const -> QRectF {
        const auto leftdown = this->leftdown();
        const auto rightup  = this->rightup();
        return QRectF{leftdown, rightup};
    }

    void Item::paintPointed(QPainter& painter) const {
        auto pen = QPen{QColor::fromRgb(0, 255, 0)};
        painter.setPen(pen);
        painter.drawRect(this->_center.x(), this->_center.y(), this->_width, this->_height);
    }

    auto Item::isPointed(const QPoint& pointed) -> bool {
        auto leftdown = this->leftdown();
        auto rightup = this->rightup();
        return 
            pointed.x() >= leftdown.x() &&
            pointed.y() >= leftdown.y() &&
            pointed.x() <= rightup.x() &&
            pointed.y() <= rightup.y();
    }

    auto Item::leftdown() const -> QPoint {
        return QPoint { 
            this->_center.x() - this->_width / 2, 
            this->_center.y() - this->_height / 2 
        };
    }

    auto Item::rightup() const -> QPoint {
        return QPoint { 
            this->_center.x() + this->_width / 2, 
            this->_center.y() + this->_height / 2 
        };
    }

}