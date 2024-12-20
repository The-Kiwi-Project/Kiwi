#pragma once

#include "qchar.h"
#include "qgraphicsscene.h"
#include "widget/layout/item/topdieinstitem.h"
#include <QGraphicsScene>

namespace kiwi::hardware {
    class TOB;
}

namespace kiwi::circuit {
    class TopDieInstance;
}

namespace kiwi::widget {

    namespace layout {
        class NetItem;
        class PinItem;
        class TopDieInstanceItem;
        class ExPortItem;
    }

    class LayoutScene : public QGraphicsScene {
    public:
        LayoutScene();

    public:
        auto addNet(layout::PinItem* beginPoint, layout::PinItem* endPoint) -> layout::NetItem*;
        auto addTOB(hardware::TOB* tob) -> layout::TOBItem*;
        auto addTopDieInst(circuit::TopDieInstance* topdieInst) -> layout::TopDieInstItem*;
        auto addExPort(const QString& name) -> layout::ExPortItem*;
    };


}