#include "./cobitem.hh"
#include "qbrush.h"
#include "qcolor.h"
#include "qnamespace.h"
#include "qpainter.h"
#include "qpen.h"
#include "qpoint.h"

namespace kiwi::widget {

    QColor COBItem::color = QColor::fromRgb(255, 128, 0); 

    COBItem::COBItem(const QPoint& offset) :
        Item{offset, COBItem::WIDTH, COBItem::WIDTH}
    {
        setFlags(ItemIsSelectable | ItemIsMovable);
    setAcceptHoverEvents(true);
    }

    void COBItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
        painter->setBrush(QBrush{COBItem::color});
        painter->drawRoundedRect(
            QRect{this->leftdown(), this->rightup()}, 
            10, 10, Qt::RelativeSize);
        painter->drawText(this->leftdown(), "hh");
    }

}