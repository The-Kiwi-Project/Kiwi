#include "./schematicscene.h"
#include "./item/netitem.h"
#include "./item/pinitem.h"
#include "./item/exportitem.h"
#include "./item/topdieinstitem.h"
#include <QGraphicsSceneMouseEvent>

namespace kiwi::widget {

    SchematicScene::SchematicScene() :
        QGraphicsScene{}
    {
    }

    void SchematicScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
        if (this->_floatingNet != nullptr) {
            this->_floatingNet->updateEndPoint(event->scenePos());
        }
        QGraphicsScene::mouseMoveEvent(event);
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
            endPoint->setNetItem(this->_floatingNet);

            this->_floatingNet = nullptr;
        } else {
            auto beginPoint = this->addNetPoint(pin);
        
            this->_floatingNet = new schematic::NetItem {beginPoint};
            this->addItem(this->_floatingNet);
        }
    }

}