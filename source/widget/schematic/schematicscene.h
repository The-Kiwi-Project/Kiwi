#pragma once

#include "qgraphicsscene.h"
#include <QGraphicsScene>

namespace kiwi::widget {

    namespace schematic {
        class NetPointItem;
        class NetItem;
        class PintItem;
        class TopDieInstanceItem;
    }

    class SchematicScene : public QGraphicsScene {
    public:
        SchematicScene();

    protected:
        void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

    public:
        auto floatingNet() -> schematic::NetItem* 
        { return this->_floatingNet; }

        void setFloatingNet(schematic::NetItem* net) 
        { this->_floatingNet = net; }

    public:
        schematic::NetItem* _floatingNet {};
    };

}