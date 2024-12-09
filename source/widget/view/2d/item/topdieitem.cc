#include "./topdieitem.hh"
#include "qbrush.h"
#include "qcolor.h"
#include "qpen.h"
#include "qpoint.h"

namespace kiwi::widget {

    QColor TopDieInstItem::color = QColor::fromRgb(50, 200, 50, 60); 

    TopDieInstItem::TopDieInstItem(const QPoint& offset) :
        Item{offset, TopDieInstItem::WIDTH, TopDieInstItem::WIDTH}
    {
    }

    void TopDieInstItem::paintSelf(QPainter& painter) const {
        painter.setPen(QPen{TopDieInstItem::color});
        painter.setBrush(QBrush{TopDieInstItem::color});
        painter.drawRect(QRect{this->leftdown(), this->rightup()});
    }

}