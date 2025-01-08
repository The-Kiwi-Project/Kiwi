#pragma once

#include "qhash.h"
#include <QGraphicsScene>

namespace kiwi::circuit {
    class TopDieInstance;
    class TopDie;
    class ExternalPort;
    class BaseDie;
    class Connection;
    class Pin;
}

namespace kiwi::widget {

    namespace schematic {
        class NetPointItem;
        class NetItem;
        class PinItem;
        class TopDieInstanceItem;
        class ExternalPortItem;
    }

    class SchematicScene : public QGraphicsScene {
        Q_OBJECT
        
    public:
        SchematicScene(circuit::BaseDie* basedie);

    signals:
        void netSelected(schematic::NetItem* net);
        void topdieInstSelected(schematic::TopDieInstanceItem* topdieinst);
        void exportSelected(schematic::ExternalPortItem* eport);
        void viewSelected();

    protected:
        void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
        void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
        void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

    public:
        auto addExPort(circuit::ExternalPort*) -> schematic::ExternalPortItem*;
        auto addTopDieInst(circuit::TopDieInstance* inst) -> schematic::TopDieInstanceItem*;
        auto addNet(circuit::Connection* connection) -> schematic::NetItem*;
        
    private:
        auto addNetPoint(schematic::PinItem* pin) -> schematic::NetPointItem*;

    public:
        /*
            Load all item from the basedie!
        */
        void initialSceneItems();

    public:
        void headleCreateNet(schematic::PinItem* pin, QGraphicsSceneMouseEvent* event);
        void handleInitialTopDie(circuit::TopDie* topdie);
        void handleAddExport();

    public:
        auto topdieinstMap() -> QHash<circuit::TopDieInstance*, schematic::TopDieInstanceItem*>& 
        { return this->_topdieinstMap; }

    private:
        void initialTopDieInstItems();
        void initialExternalPortItems();
        void initialNetItems();

    private:
        auto circuitPinToPinItem(const circuit::Pin& pin) -> schematic::PinItem*;

    private:
        void placeFloatingTopdDieInst();
        void cleanFloatingTopdDieInst();

        void placeFloatingExPort();
        void cleanFloatingExPort();

    protected:
        circuit::BaseDie* _basedie;

        QHash<circuit::TopDieInstance*, schematic::TopDieInstanceItem*> _topdieinstMap;
        QHash<circuit::ExternalPort*, schematic::ExternalPortItem*> _exportMap;

        // Temp var 
        schematic::NetItem* _floatingNet {nullptr};
        schematic::TopDieInstanceItem* _floatingTopdDieInst {nullptr};
        schematic::ExternalPortItem* _floatingExPort {nullptr};
    };

}