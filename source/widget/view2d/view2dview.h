#pragma once

#include "./view2dscene.hh"
#include <widget/frame/graphicsview.h>
#include "hardware/cob/cobcoord.hh"
#include "hardware/track/trackcoord.hh"
#include "qpoint.h"
#include "std/utility.hh"
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
   
    class View2DView : public GraphicsView {

        static constexpr qreal COB_WIDTH_INTERVAL = 250.; 
        static constexpr qreal COB_HEIGHT_INTERVAL = 250.; 

    public:
        explicit View2DView(
            hardware::Interposer* interposer, 
            circuit::BaseDie* basedie,
            QWidget *parent = nullptr);

        ~View2DView() noexcept;

    public:
        auto displayCOBConnections() -> void;

    protected:
        void addTrack(const hardware::TrackCoord& coord);
        void addTrack(const QPointF& begin, const QPointF& end);

        static auto trackPositions(const hardware::TrackCoord& coord) -> std::Tuple<QPointF, QPointF>;

    protected:
        static auto cobPosition(const hardware::COBCoord& coord) -> QPointF;
        static auto tobPosition(const hardware::COBCoord& coord) -> QPointF;

    protected:
        hardware::Interposer* _interposer {nullptr};
        circuit::BaseDie* _basedie {nullptr};

        View2DScene* _scene {nullptr};
    };

}