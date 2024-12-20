#pragma once

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

    class LayoutView : public QGraphicsView {
        
        static constexpr qreal TOB_INTERVAL = 30.0;
        static const     QColor BACK_COLOR;

    public:
        explicit LayoutView(
            hardware::Interposer* interposer, 
            circuit::BaseDie* basedie,
            QWidget *parent = nullptr);

        ~LayoutView() noexcept;

    protected:
        static auto tobPosition(const hardware::TOBCoord& coord) -> QPointF;

    protected:
        virtual void wheelEvent(QWheelEvent *event) override;

    protected:
        hardware::Interposer* _interposer {nullptr};
        circuit::BaseDie* _basedie {nullptr};

        LayoutScene* _scene {nullptr};
    };

}