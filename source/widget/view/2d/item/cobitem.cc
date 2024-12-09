#include "./cobitem.hh"
#include "qbrush.h"
#include "qcolor.h"
#include "qpainter.h"
#include "qpen.h"
#include "qpoint.h"

namespace kiwi::widget {

    QColor COBItem::color = QColor::fromRgb(200, 50, 120); 

    COBItem::COBItem(const QPoint& offset) :
        Item{offset, COBItem::WIDTH, COBItem::WIDTH}
    {
    }

    void COBItem::paintSelf(QPainter& painter) const {
        painter.setPen(QPen{COBItem::color});
        painter.setBrush(QBrush{COBItem::color});
        painter.drawRect(QRect{this->leftdown(), this->rightup()});
    }

}