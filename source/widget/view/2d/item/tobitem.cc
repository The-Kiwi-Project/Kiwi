#include "./tobitem.hh"
#include "qbrush.h"
#include "qcolor.h"
#include "qpen.h"
#include "qpoint.h"

namespace kiwi::widget {

    QColor TOBItem::color = QColor::fromRgb(50, 50, 200); 

    TOBItem::TOBItem(const QPoint& offset) :
        Item{offset, TOBItem::WIDTH, TOBItem::WIDTH}
    {
    }

    void TOBItem::paintSelf(QPainter& painter) const {
        painter.setPen(QPen{TOBItem::color});
        painter.setBrush(QBrush{TOBItem::color});
        painter.drawRect(QRect{this->leftdown(), this->rightup()});
    }

}