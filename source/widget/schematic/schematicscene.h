#pragma once

#include "qchar.h"
#include "qhash.h"
#include "qset.h"
#include "qvector.h"
#include "widget/schematic/item/sourceportitem.h"
#include <QGraphicsScene>

namespace kiwi::circuit {
    class TopDieInstance;
    class TopDie;
    class ExternalPort;
    class BaseDie;
    class Connection;
    class Pin;
}

namespace kiwi::hardware {
    class Interposer;
}

namespace kiwi::widget {

    namespace schematic {
        class NetPointItem;
        class NetItem;
        class PinItem;
        class TopDieInstanceItem;
        class ExternalPortItem;
        class SourcePortItem;
    }

    class SchematicScene : public QGraphicsScene {
        Q_OBJECT
        
    public:
        SchematicScene(circuit::BaseDie* basedie, hardware::Interposer* interposer);

    signals:
        void netSelected(schematic::NetItem* net);
        void topdieInstSelected(schematic::TopDieInstanceItem* topdieinst);
        void exportSelected(schematic::ExternalPortItem* eport);
        void viewSelected();
        void layoutChanged();

    public:
        void reloadItems();

    protected:
        void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
        void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
        void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

    public:
        auto addExPort(circuit::ExternalPort*) -> schematic::ExternalPortItem*;
        auto addTopDieInst(circuit::TopDieInstance* inst) -> schematic::TopDieInstanceItem*;
        auto addNet(circuit::Connection* connection) -> schematic::NetItem*;
        auto addVDDSourcePort(const QString& name) -> schematic::SourcePortItem*;
        auto addGNDSourcePort(const QString& name) -> schematic::SourcePortItem*;

    public:
        void removeExternalPort(schematic::ExternalPortItem* eport);
        void removeTopDieInstance(schematic::TopDieInstanceItem* inst);
        void removeNet(schematic::NetItem* net);
        
    private:
        auto addNetPoint(schematic::PinItem* pin) -> schematic::NetPointItem*;

    public:
        /*
            Load all item from the basedie!
        */
        void addSceneItems();

    public:
        void headleCreateNet(schematic::PinItem* pin, QGraphicsSceneMouseEvent* event);
        void handleInitialTopDie(circuit::TopDie* topdie);
        void handleAddExport();

    public:
        auto topdieinstMap() -> QHash<circuit::TopDieInstance*, schematic::TopDieInstanceItem*>& 
        { return this->_topdieinstMap; }

    private:
        void addTopDieInstItems();
        void addExternalPortItems();
        void addNetItems();

    private:
        auto circuitPinToPinItem(const circuit::Pin& pin) -> schematic::PinItem*;

    private:
        void placeFloatingTopdDieInst();
        void cleanFloatingTopdDieInst();

        void placeFloatingExPort();
        void cleanFloatingExPort();

    protected:
        circuit::BaseDie* _basedie;
        hardware::Interposer* _interposer;

        QHash<circuit::TopDieInstance*, schematic::TopDieInstanceItem*> _topdieinstMap;
        QHash<circuit::ExternalPort*, schematic::ExternalPortItem*> _exportMap;
        QSet<schematic::NetItem*> _nets;
        QVector<schematic::SourcePortItem*> _vddPorts;
        QVector<schematic::SourcePortItem*> _gndPorts;

        // Temp var 
        schematic::NetItem* _floatingNet {nullptr};
        schematic::TopDieInstanceItem* _floatingTopdDieInst {nullptr};
        schematic::ExternalPortItem* _floatingExPort {nullptr};
    };

}