#pragma once

#include <QGraphicsScene>

namespace kiwi::circuit {
    class TopDieInstance;
}

namespace kiwi::widget {

    namespace schematic {
        class NetPointItem;
        class NetItem;
        class PinItem;
        class TopDieInstanceItem;
        class ExPortItem;
    }

    class SchematicScene : public QGraphicsScene {
        Q_OBJECT
        
    public:
        SchematicScene();

    signals:
        void netSelected(schematic::NetItem* net);
        void topdieInstSelected(schematic::TopDieInstanceItem* topdieinst);
        void viewSelected();

    protected:
        void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
        void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
        void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

    public:
        auto addNetPoint(schematic::PinItem* pin) -> schematic::NetPointItem*;
        auto addNet(schematic::NetPointItem* beginPoint, schematic::NetPointItem* endPoint) -> schematic::NetItem*;
        auto addExPort(const QString& name) -> schematic::ExPortItem*;
        auto addTopDieInst(circuit::TopDieInstance* inst) -> schematic::TopDieInstanceItem*;

    public:
        auto connectPins(schematic::PinItem* begin, schematic::PinItem* end) -> schematic::NetItem*;

        void headleCreateNet(schematic::PinItem* pin, QGraphicsSceneMouseEvent* event);

    public:
        auto topdieinstMap() -> QHash<circuit::TopDieInstance*, schematic::TopDieInstanceItem*>& 
        { return this->_topdieinstMap; }

    protected:
        schematic::NetItem* _floatingNet {};

        QHash<circuit::TopDieInstance*, schematic::TopDieInstanceItem*> _topdieinstMap;
    };

}