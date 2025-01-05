#include "./schematicscene.h"
#include "./item/netitem.h"
#include "./item/netpointitem.h"
#include "./item/pinitem.h"
#include "./item/exportitem.h"
#include "./item/topdieinstitem.h"
#include "circuit/topdieinst/topdieinst.hh"
#include <circuit/basedie.hh>
#include "qchar.h"
#include "qnamespace.h"
#include "widget/schematic/item/griditem.h"
#include <QGraphicsSceneMouseEvent>
#include <format>

namespace kiwi::widget {

    static_assert((int)schematic::NetItem::Type != (int)schematic::PinItem::Type);
    static_assert((int)schematic::NetItem::Type != (int)schematic::TopDieInstanceItem::Type);

    SchematicScene::SchematicScene(circuit::BaseDie* basedie) :
        _basedie{basedie},
        QGraphicsScene{}
    {
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
        }

        QGraphicsScene::mouseDoubleClickEvent(event);
    }

    auto SchematicScene::addNetPoint(schematic::PinItem* pin) -> schematic::NetPointItem* {
        auto point = new schematic::NetPointItem {pin};
        this->addItem(point);
        return point;
    }

    auto SchematicScene::addNet(schematic::NetPointItem* beginPoint, schematic::NetPointItem* endPoint) -> schematic::NetItem* {
        auto net = new schematic::NetItem {beginPoint, endPoint};
        this->addItem(net);
        return net;
    }

    auto SchematicScene::addExPort(const QString& name) -> schematic::ExPortItem* {
        auto port = new schematic::ExPortItem {name, this};
        this->addItem(port);
        return port;
    }

    auto SchematicScene::addTopDieInst(circuit::TopDieInstance* inst) -> schematic::TopDieInstanceItem* {
        auto t = new schematic::TopDieInstanceItem{inst, this};
        this->_topdieinstMap.insert(inst, t);
        this->addItem(t);
        return t;
    }

    auto SchematicScene::connectPins(schematic::PinItem* begin, schematic::PinItem* end) -> schematic::NetItem* {
        auto beginPoint = this->addNetPoint(begin);
        auto endPoint = this->addNetPoint(end);
        return this->addNet(beginPoint, endPoint);
    }

    void SchematicScene::headleCreateNet(schematic::PinItem* pin, QGraphicsSceneMouseEvent* event) {
        if (this->_floatingNet != nullptr) {
            auto endPoint = this->addNetPoint(pin);
            this->_floatingNet->setEndPoint(endPoint);
            this->_floatingNet->resetPaint();
            endPoint->setNetItem(this->_floatingNet);

            this->_floatingNet = nullptr;
        } 
        else {
            auto beginPoint = this->addNetPoint(pin);
            
            this->_floatingNet = new schematic::NetItem {beginPoint};
            this->addItem(this->_floatingNet);
        }
    }

    void SchematicScene::handleInitialTopDie(circuit::TopDie* topdie) {
        // Get name!
        auto size = 0;
        for (const auto& [name, inst] : this->_basedie->topdie_insts()) {
            if (inst->topdie() == topdie) {
                size += 1;
            }
        }

        auto instName = std::format("{}_{}", topdie->name(), size);
        auto topdieInst = this->_basedie->add_topdie_inst(instName, topdie, nullptr);
        auto topdieInstItem = this->addTopDieInst(topdieInst);

        if (this->_floatingTopdDieInst != nullptr) {
            this->cleanFloatingTopdDieInst();
        }

        this->_floatingTopdDieInst = topdieInstItem;
    }

    void SchematicScene::cleanFloatingTopdDieInst() {
        assert(this->_floatingTopdDieInst != nullptr);
        this->_basedie->remove_topdie_inst(this->_floatingTopdDieInst->topdieInst()->name());
        this->removeItem(this->_floatingTopdDieInst);
        this->_floatingTopdDieInst = nullptr;
    }

    void SchematicScene::placeFloatingTopdDieInst() {
        assert(this->_floatingTopdDieInst != nullptr);
        this->_floatingTopdDieInst = nullptr;
    }
}