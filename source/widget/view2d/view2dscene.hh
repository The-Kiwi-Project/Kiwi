#pragma once

#include "qpoint.h"
#include <QGraphicsScene>

namespace kiwi::hardware {
    class TOB;
    class COB;
}

namespace kiwi::widget {

    namespace view2d {
        class BumpItem;
        class COBItem;
        class TOBItem;
        class NetItem;
    }

    class View2DScene : public QGraphicsScene {
    public:
        View2DScene();

    public:
        auto addTOB(hardware::TOB* tob) -> view2d::TOBItem*; 
        auto addCOB(hardware::COB* cob) -> view2d::COBItem*; 
        auto addNet(const QPointF& begin, const QPointF& end) -> view2d::NetItem*;
    };

}