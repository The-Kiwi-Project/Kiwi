#pragma once

#include "./item/topdieinstitem.h"
#include "./item/pinitem.h"
#include "./item/netitem.h"

#include <QWidget>
#include <QGraphicsView>
#include <std/collection.hh>

namespace kiwi::hardware {
    class Interposer;
    class TOB;
    class COB;
    class Track;
};

namespace kiwi::circuit {
    class BaseDie;
    class TopDieInstance;
};

namespace kiwi::widget {
   
    class SchematicWidget : public QGraphicsView {
        
        enum {
            COB_INTERVAL = 50,
        };

    public:
        explicit SchematicWidget(
            hardware::Interposer* interposer, 
            circuit::BaseDie* basedie,
            QWidget *parent = nullptr);

        ~SchematicWidget() noexcept;

    protected:
        virtual void wheelEvent(QWheelEvent *event) override;

    public:
        auto connectPins(schematic::PinItem* begin, schematic::PinItem* end) -> schematic::NetItem*;

    protected:
        hardware::Interposer* _interposer {nullptr};
        circuit::BaseDie* _basedie {nullptr};

        SchematicScene* _scene {nullptr};

        QVector<schematic::TopDieInstanceItem*> _topdieInstItems {};
    };

}