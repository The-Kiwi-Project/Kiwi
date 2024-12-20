#include "./layoutscene.h"
#include "./item/netitem.h"
#include "./item/pinitem.h"
#include "./item/tobitem.h"
#include "./item/topdieinstitem.h"

namespace kiwi::widget {

    LayoutScene::LayoutScene() {}

    auto LayoutScene::addNet(layout::PinItem* beginPoint, layout::PinItem* endPoint) -> layout::NetItem* {
        auto n = new layout::NetItem {beginPoint, endPoint};
        this->addItem(n);
        return n;
    }

    auto LayoutScene::addTOB(hardware::TOB* tob) -> layout::TOBItem* {
        auto t = new layout::TOBItem {tob};
        this->addItem(t);
        return t;
    }

    auto LayoutScene::addTopDieInst(circuit::TopDieInstance* topdieInst) -> layout::TopDieInstItem* {
        auto ti = new layout::TopDieInstItem {topdieInst};
        this->addItem(ti);
        return ti;
    }

    // auto LayoutScene::addExPort(const QString& name) -> layout::ExPortItem* {

    // }
}