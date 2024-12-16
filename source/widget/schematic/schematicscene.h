#pragma once

#include "qgraphicsscene.h"
#include <QGraphicsScene>

namespace kiwi::widget {

    namespace schematic {
        class NetPointItem;
        class NetItem;
        class PinItem;
        class TopDieInstanceItem;
    }

    class SchematicScene : public QGraphicsScene {
    public:
        SchematicScene();

    protected:
        void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

    public:
        auto addNetPoint(schematic::PinItem* pin) -> schematic::NetPointItem*;
        auto addNet(schematic::NetPointItem* beginPoint, schematic::NetPointItem* endPoint) -> schematic::NetItem*;
        
        auto connectPins(schematic::PinItem* begin, schematic::PinItem* end) -> schematic::NetItem*;

        void headleCreateNet(schematic::PinItem* pin, QGraphicsSceneMouseEvent* event);

    protected:
        schematic::NetItem* _floatingNet {};
    };

}