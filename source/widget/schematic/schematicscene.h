#pragma once

#include "circuit/connection/connection.hh"
#include "circuit/connection/pin.hh"
#include <QGraphicsScene>

namespace kiwi::circuit {
    class TopDieInstance;
    class TopDie;
    class ExternalPort;
    class BaseDie;
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
        SchematicScene(circuit::BaseDie* basedie);

    signals:
        void netSelected(schematic::NetItem* net);
        void topdieInstSelected(schematic::TopDieInstanceItem* topdieinst);
        void exportSelected(schematic::ExPortItem* eport);
        void viewSelected();

    protected:
        void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
        void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
        void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

    public:
        auto addExPort(circuit::ExternalPort*) -> schematic::ExPortItem*;
        auto addTopDieInst(circuit::TopDieInstance* inst) -> schematic::TopDieInstanceItem*;
        auto connectPins(schematic::PinItem* begin, schematic::PinItem* end, int sync = -1) -> schematic::NetItem*;

        auto addNetPoint(schematic::PinItem* pin) -> schematic::NetPointItem*;
        auto addNet(circuit::Connection* connection, schematic::NetPointItem* beginPoint, schematic::NetPointItem* endPoint) -> schematic::NetItem*;

    public:
        void headleCreateNet(schematic::PinItem* pin, QGraphicsSceneMouseEvent* event);
        void handleInitialTopDie(circuit::TopDie* topdie);
        void handleAddExport();

    public:
        auto topdieinstMap() -> QHash<circuit::TopDieInstance*, schematic::TopDieInstanceItem*>& 
        { return this->_topdieinstMap; }

    private:
        auto getCircuitPin(schematic::PinItem* pin) -> circuit::Pin;

        void placeFloatingTopdDieInst();
        void cleanFloatingTopdDieInst();

        void placeFloatingExPort();
        void cleanFloatingExPort();

    protected:
        circuit::BaseDie* _basedie;

        schematic::NetItem* _floatingNet {nullptr};
        schematic::TopDieInstanceItem* _floatingTopdDieInst {nullptr};
        schematic::ExPortItem* _floatingExPort {nullptr};

        QHash<circuit::TopDieInstance*, schematic::TopDieInstanceItem*> _topdieinstMap;
    };

}