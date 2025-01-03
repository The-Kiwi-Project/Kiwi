#pragma once

#include "./item/topdieinstitem.h"
#include "./item/pinitem.h"
#include "qglobal.h"

#include <widget/frame/graphicsview.h>
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
   
    class SchematicView : public GraphicsView {
        
        enum {
            COB_INTERVAL = 50,
        };

    public:
        explicit SchematicView(
            hardware::Interposer* interposer, 
            circuit::BaseDie* basedie,
            QWidget *parent = nullptr);

        ~SchematicView() noexcept;

    protected:
        void drawBackground(QPainter* painter, const QRectF& rect) override;

    protected:
        hardware::Interposer* _interposer {nullptr};
        circuit::BaseDie* _basedie {nullptr};

        SchematicScene* _scene {nullptr};

        QVector<schematic::TopDieInstanceItem*> _topdieInstItems {};

        qreal _gridSize{20};
    };

}