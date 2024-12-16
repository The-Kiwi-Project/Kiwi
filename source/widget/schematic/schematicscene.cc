#include "./schematicscene.h"
#include "./item/netitem.h"
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

}