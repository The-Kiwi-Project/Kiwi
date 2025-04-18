#include "./schematicscene.h"

#include "./item/netitem.h"
#include "./item/netpointitem.h"
#include "./item/pinitem.h"
#include "./item/exportitem.h"
#include "./item/topdieinstitem.h"
#include "./item/griditem.h"
#include "./item/sourceportitem.h"

#include <circuit/connection/pin.hh>
#include <circuit/connection/connection.hh>
#include <circuit/topdieinst/topdieinst.hh>
#include <circuit/basedie.hh>
#include <hardware/interposer.hh>
#include <widget/frame/itemtypecheck.h>

#include <debug/debug.hh>
#include <QMessageBox>
#include <QGraphicsSceneMouseEvent>

namespace kiwi::widget {

    static_assert(AllUnique<
        (int)schematic::NetItem::Type,
        (int)schematic::PinItem::Type,
        (int)schematic::TopDieInstanceItem::Type,
        (int)schematic::NetPointItem::Type,
        (int)schematic::ExternalPortItem::Type,
        (int)schematic::SourcePortItem::Type
    >::value);

    SchematicScene::SchematicScene(circuit::BaseDie* basedie, hardware::Interposer* interposer) :
        _basedie{basedie},
        _interposer{interposer},
        QGraphicsScene{}
    {
        this->addSceneItems();
    }

    void SchematicScene::reloadItems() {
        // Clear
        this->_topdieinstMap.clear();
        this->_exportMap.clear();
        this->_nets.clear();
        this->_vddPorts.clear();
        this->_gndPorts.clear();

        this->_floatingNet = nullptr;
        this->_floatingTopdDieInst = nullptr;
        this->_floatingExPort = nullptr;

        this->clear();

        this->addSceneItems();
    }

    void SchematicScene::addSceneItems() {
        this->addTopDieInstItems();
        this->addExternalPortItems();
        this->addNetItems();
    }

    void SchematicScene::addTopDieInstItems() {
        // Load all topdie inst!
        for (auto& [name, topdie] : this->_basedie->topdie_insts()) {
            auto t = this->addTopDieInst(topdie.get());
        }

        auto spacing = schematic::GridItem::GRID_SIZE;

        int cols = std::ceil(std::sqrt(this->_topdieinstMap.size()));
        int rows = std::ceil(static_cast<double>(this->_topdieinstMap.size()) / cols);

        int startX = spacing;
        int startY = spacing;

        auto i = 0;
        for (auto& topdieInstItems : this->_topdieinstMap) {
            int row = i / cols;
            int col = i % cols;

            int x = startX + col * (topdieInstItems->width() + spacing);
            int y = startY + row * (topdieInstItems->height() + spacing);

            topdieInstItems->setPos(x, y);

            i += 1;
        }
    }

    void SchematicScene::addExternalPortItems() {
        auto i = 0;
        for (auto& [name, eport] : this->_basedie->external_ports()) {
            auto p = this->addExPort(eport.get());
            p->setPos(QPointF{
                -20. * schematic::GridItem::GRID_SIZE, 
                i * (schematic::ExternalPortItem::HEIGHT + schematic::GridItem::GRID_SIZE)
            });

            i += 1;
        }
    }

    void SchematicScene::addNetItems() {
        for (auto& [sync, connections] : this->_basedie->connections()) {
            for (const auto& connection : connections) {
                this->addNet(connection.get());
            }
        }
    }

    void SchematicScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
        if (this->_floatingNet != nullptr) {
            auto gridPos = schematic::GridItem::snapToGrid(event->scenePos());
            this->_floatingNet->updateEndPoint(gridPos);
        }
        if (this->_floatingTopdDieInst != nullptr) {
            auto gridPos = schematic::GridItem::snapToGrid(event->scenePos());
            this->_floatingTopdDieInst->setPos(gridPos);
        }
        if (this->_floatingExPort != nullptr) {
            auto gridPos = schematic::GridItem::snapToGrid(event->scenePos());
            this->_floatingExPort->setPos(gridPos);
        }
        QGraphicsScene::mouseMoveEvent(event);
    }

    void SchematicScene::mousePressEvent(QGraphicsSceneMouseEvent* event) {
        if (this->_floatingNet != nullptr) {
            if (event->button() & Qt::LeftButton) {
                auto gridPos = schematic::GridItem::snapToGrid(event->scenePos());
                this->_floatingNet->addPoint(gridPos);
            }
            else if (event->button() & Qt::RightButton) {
                this->_floatingNet->beginPoint()->unlinkPin();
                this->items().removeOne(this->_floatingNet);
                delete this->_floatingNet->beginPoint();
                delete this->_floatingNet;
                this->_floatingNet = nullptr;
            }
        }

        if (this->_floatingTopdDieInst != nullptr) {
            if (event->button() & Qt::LeftButton) {
                this->placeFloatingTopdDieInst();
            }
            else if (event->button() & Qt::RightButton) {
                this->cleanFloatingTopdDieInst();
            }
        }
        
        if (this->_floatingExPort != nullptr) {
            if (event->button() & Qt::LeftButton) {
                this->placeFloatingExPort();
            }
            else if (event->button() & Qt::RightButton) {
                this->cleanFloatingExPort();
            }
        }

        QGraphicsScene::mousePressEvent(event);
    }

    void SchematicScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) {
        QGraphicsItem *item = itemAt(event->scenePos(), QTransform());
        if (!item) {
            emit this->viewSelected();
        }
        else {
            if (item->type() == schematic::NetItem::Type) {
                emit this->netSelected(dynamic_cast<schematic::NetItem*>(item));
            }
            else if (item->type() == schematic::TopDieInstanceItem::Type) {
                emit this->topdieInstSelected(dynamic_cast<schematic::TopDieInstanceItem*>(item));
            }
            else if (item->type() == schematic::ExternalPortItem::Type) {
                emit this->exportSelected(dynamic_cast<schematic::ExternalPortItem*>(item));
            }
        }

        QGraphicsScene::mouseDoubleClickEvent(event);
    }

    auto SchematicScene::addNetPoint(schematic::PinItem* pin) -> schematic::NetPointItem* {
        auto point = new schematic::NetPointItem {pin};
        this->addItem(point);
        return point;
    }

    auto SchematicScene::addExPort(circuit::ExternalPort* eport) -> schematic::ExternalPortItem* {
        auto item = new schematic::ExternalPortItem {eport};
        this->_exportMap.insert(eport, item);
        this->addItem(item);
        return item;
    }

    auto SchematicScene::addTopDieInst(circuit::TopDieInstance* inst) -> schematic::TopDieInstanceItem* {
        auto item = new schematic::TopDieInstanceItem{inst};
        this->_topdieinstMap.insert(inst, item);
        this->addItem(item);
        return item;
    }

    auto SchematicScene::addNet(circuit::Connection* connection) -> schematic::NetItem* {
        auto beginPin = this->circuitPinToPinItem(connection->input_pin());
        auto endPin = this->circuitPinToPinItem(connection->output_pin());

        auto beginPoint = this->addNetPoint(beginPin);
        auto endPoint = this->addNetPoint(endPin);

        auto item = new schematic::NetItem {connection, beginPoint, endPoint};
        this->addItem(item);
        this->_nets.insert(item);

        return item;
    }

    auto SchematicScene::addVDDSourcePort(const QString& name) -> schematic::SourcePortItem* {
        auto item = new schematic::SourcePortItem{name, schematic::SourcePortType::VDD};
        this->_vddPorts.push_back(item);
        this->addItem(item);
        
        auto x = 0.;
        for (auto i : this->_vddPorts) {
            x += (i->width() + 2 * schematic::GridItem::GRID_SIZE);
        }

        item->setPos(QPointF{x, -5 * schematic::GridItem::GRID_SIZE});

        return item;
    }

    auto SchematicScene::addGNDSourcePort(const QString& name) -> schematic::SourcePortItem* {
        auto item = new schematic::SourcePortItem{name, schematic::SourcePortType::GND};
        this->_gndPorts.push_back(item);
        this->addItem(item);
        
        auto x = 0.;
        for (auto i : this->_gndPorts) {
            x += (i->width() + 2 * schematic::GridItem::GRID_SIZE);
        }

        item->setPos(QPointF{x, -10 * schematic::GridItem::GRID_SIZE});

        return item;
    }

    void SchematicScene::removeExternalPort(schematic::ExternalPortItem* eport) {
        assert(eport != nullptr);
        // debug::debug_fmt("Remove external port '{}'", eport->exPort()->name());

        auto pinItem = eport->pin();
        assert(pinItem != nullptr);
        
        for (auto pointItem : pinItem->connectedPoints()) {
            auto netItem = pointItem->netItem();
            assert(netItem != nullptr);
            this->removeNet(netItem);
        }

        delete pinItem;

        this->_exportMap.remove(eport->unwrap());
        this->removeItem(eport);

        delete eport;
    }

    void SchematicScene::removeTopDieInstance(schematic::TopDieInstanceItem* inst) {
        assert(inst != nullptr);
        // debug::debug_fmt("Remove topdie instance '{}'", inst->topdieInst()->name());

        for (auto pinItem : inst->pins()) {
            for (auto point : pinItem->connectedPoints()) {
                assert(point != nullptr);
                assert(point->netItem() != nullptr);
                this->removeNet(point->netItem());
            }
            delete pinItem;
        }

        this->_topdieinstMap.remove(inst->unwrap());
        this->removeItem(inst);

        delete inst;
    }

    void SchematicScene::removeNet(schematic::NetItem* net) {
        // net has two point, each pin has 
        assert(net != nullptr);
        // debug::debug_fmt("Remove net '{}'", *net->connection());
    
        auto beginPoint = net->beginPoint();
        auto endPoint = net->endPoint();

        assert(beginPoint != nullptr && endPoint != nullptr);

        auto beginPin = beginPoint->connectedPin();
        auto endPin = endPoint->connectedPin();

        assert(beginPoint != nullptr && endPoint != nullptr);

        beginPin->removeConnectedPoint(beginPoint);
        endPin->removeConnectedPoint(endPoint);

        this->removeItem(net);
        this->_nets.remove(net);

        delete net;
        delete beginPoint;
        delete endPoint;
    }

    auto SchematicScene::circuitPinToPinItem(const circuit::Pin& pin) -> schematic::PinItem* {
        return std::match(pin.connected_point(),
            [this](const circuit::ConnectVDD& vdd) -> schematic::PinItem* {
                return this->addVDDSourcePort(QString::fromStdString(vdd.name))->pin();
            },
            [this](const circuit::ConnectGND& gnd) -> schematic::PinItem* {
                return this->addGNDSourcePort(QString::fromStdString(gnd.name))->pin();
            },
            [this](const circuit::ConnectExPort& eport) -> schematic::PinItem* {
                auto eportItem = this->_exportMap.value(eport.port);
                return eportItem->pin();
            },
            [this](const circuit::ConnectBump& bump) -> schematic::PinItem* {
                auto top = this->_topdieinstMap.value(bump.inst);
                return top->pins().value(QString::fromStdString(bump.name));
            }
        );
    }

    void SchematicScene::headleCreateNet(schematic::PinItem* pin, QGraphicsSceneMouseEvent* event) {
        if (this->_floatingNet != nullptr) {
            auto endPoint = this->addNetPoint(pin);
            
            auto connection = this->_basedie->add_connection(
                -1, 
                this->_floatingNet->beginPoint()->connectedPin()->toCircuitPin(),
                endPoint->connectedPin()->toCircuitPin()
            );

            this->_floatingNet->wrap(connection);
            this->_floatingNet->setEndPoint(endPoint);

            this->_floatingNet->resetPaint();
            endPoint->setNetItem(this->_floatingNet);
            this->_nets.insert(this->_floatingNet);

            this->_floatingNet = nullptr;

            emit this->layoutChanged();
        } 
        else {
            auto beginPoint = this->addNetPoint(pin);
            
            this->_floatingNet = new schematic::NetItem {beginPoint};
            this->addItem(this->_floatingNet);
        }
    }

    void SchematicScene::handleInitialTopDie(circuit::TopDie* topdie) {
        auto idle_tob = this->_interposer->get_a_idle_tob();
        if (!idle_tob.has_value()) {
            QMessageBox::critical(
                nullptr,
                "Add TopDies Error",
                "No idle tob to place topdie inst!"
            );
        } 
        else {
            auto topdieInst = this->_basedie->add_topdie_inst(topdie, *idle_tob);
            auto topdieInstItem = this->addTopDieInst(topdieInst);

            if (this->_floatingTopdDieInst != nullptr) {
                this->cleanFloatingTopdDieInst();
            }

            this->_floatingTopdDieInst = topdieInstItem;

            emit this->layoutChanged();
        }
    }

    void SchematicScene::handleAddExport() {
        auto eport = this->_basedie->add_external_port({});
        auto eportItem = this->addExPort(eport);

        if (this->_floatingExPort != nullptr) {
            this->cleanFloatingExPort();
        }

        this->_floatingExPort = eportItem;

        emit this->layoutChanged();
    }

    void SchematicScene::placeFloatingTopdDieInst() {
        assert(this->_floatingTopdDieInst != nullptr);
        this->_floatingTopdDieInst = nullptr;
    }

    void SchematicScene::cleanFloatingTopdDieInst() {
        assert(this->_floatingTopdDieInst != nullptr);
        assert(this->_floatingTopdDieInst->unwrap() != nullptr);

        this->_basedie->remove_topdie_inst(this->_floatingTopdDieInst->unwrap());
        this->removeItem(this->_floatingTopdDieInst);
        this->_floatingTopdDieInst = nullptr;

        emit this->layoutChanged();
    }

    void SchematicScene::placeFloatingExPort() {
        assert(this->_floatingExPort != nullptr);
        this->_floatingExPort = nullptr;
    }

    void SchematicScene::cleanFloatingExPort() {
        assert(this->_floatingExPort != nullptr);
        assert(this->_floatingExPort->unwrap() != nullptr);

        this->_basedie->remove_external_port(this->_floatingExPort->unwrap());
        this->removeItem(this->_floatingExPort);
        this->_floatingExPort = nullptr;

        emit this->layoutChanged();
    }

}