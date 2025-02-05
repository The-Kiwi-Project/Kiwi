#include "./view2dscene.hh"
#include "./item/cobitem.h"
#include "./item/tobitem.h"
#include "widget/view2d/item/netitem.h"
#include <hardware/bump/bump.hh>
#include <QGraphicsScene>

namespace kiwi::widget {

    View2DScene::View2DScene() :
        QGraphicsScene{}
    {
    }

    auto View2DScene::addTOB(hardware::TOB* tob) -> view2d::TOBItem* {
        auto item = new view2d::TOBItem{tob};
        this->addItem(item);
        return item;
    } 

    auto View2DScene::addCOB(hardware::COB* cob) -> view2d::COBItem* {
        auto item = new view2d::COBItem{cob};
        this->addItem(item);
        return item;
    }

    auto View2DScene::addNet(const QPointF& begin, const QPointF& end) -> view2d::NetItem* {
        auto item = new view2d::NetItem{begin, end};
        this->addItem(item);
        return item;
    }
    
}