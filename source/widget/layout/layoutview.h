#pragma once

#include <widget/frame/graphicsview.h>
#include "hardware/tob/tobcoord.hh"
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
   
    class LayoutScene;

    class LayoutView : public GraphicsView {
        static const     QColor BACK_COLOR;

    public:
        explicit LayoutView(
            hardware::Interposer* interposer, 
            circuit::BaseDie* basedie,
            QWidget *parent = nullptr);

        ~LayoutView() noexcept;
        
    protected:
        hardware::Interposer* _interposer {nullptr};
        circuit::BaseDie* _basedie {nullptr};

        LayoutScene* _scene {nullptr};
    };

}