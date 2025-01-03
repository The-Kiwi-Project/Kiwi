#include "./tobitem.h"
#include "./bumpitem.h"
#include <hardware/bump/bump.hh>

#include "qcolor.h"
#include "qpoint.h"

namespace kiwi::widget::view2d {

    /*
    
        +---------------------------------------+
        |                                       |
        |    * * * * * * * * * * * * * * * *    |
        |    * * * * * * * * * * * * * * * *    |
        |    * * * * * * * * * * * * * * * *    |
        |    * * * * * * * * * * * * * * * *    |
        |    * * * * * * * * * * * * * * * *    |
        |    * * * * * * * * * * * * * * * *    |
        |    * * * * * * * * * * * * * * * *    |
        |    * * * * * * * * * * * * * * * *    |
        |                                       |
        +---------------------------------------+
    
    */

    const QColor TOBItem::COLOR = QColor::fromRgb(84, 139, 84);

    TOBItem::TOBItem(hardware::TOB* tob): 
        _tob{tob} 
    {   
        // Add bump
        auto basePos = QPointF{-BUMP_AREA_WIDTH / 2., -BUMP_AREA_HEIGHT / 2.};
        for (int r = 0; r < BUMP_ARRAY_ROW; ++r) {
            for (int c = 0; c < BUMP_ARRAY_COL; ++c) {
                auto bumpItem = new BumpItem {
                    nullptr, 
                    basePos + QPointF{
                    c * BUMP_HORI_INTERVAL, r * BUMP_VERT_INTERVAL
                    },
                    this
                };
            }
        }
    }

    auto TOBItem::boundingRect() const -> QRectF {
        return QRectF(-WIDTH / 2., -HEIGHT / 2., WIDTH, HEIGHT);
    }
    
    void TOBItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
        painter->setBrush(COLOR);
        painter->drawRect(this->boundingRect());
    }


}