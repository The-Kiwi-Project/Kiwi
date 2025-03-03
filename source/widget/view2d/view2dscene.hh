#pragma once

#include <QGraphicsScene>
#include <hardware/cob/cobcoord.hh>
#include <hardware/track/trackcoord.hh>
#include <std/utility.hh>

namespace kiwi::hardware {
    class TOB;
    class COB;
    class Interposer;
}

namespace kiwi::circuit {
    class BaseDie;
    class TopDieInstance;
};

namespace kiwi::widget {

    namespace view2d {
        class BumpItem;
        class COBItem;
        class TOBItem;
        class NetItem;
    }

    class View2DScene : public QGraphicsScene {

        static constexpr qreal COB_WIDTH_INTERVAL = 250.; 
        static constexpr qreal COB_HEIGHT_INTERVAL = 250.; 

    public:
        View2DScene(
            hardware::Interposer* interposer, 
            circuit::BaseDie* basedie);

    public:
        void reloadItems();

    protected:
        void addSceneItems();
        void displayRoutingResult();

    public:
        auto addTOB(hardware::TOB* tob) -> view2d::TOBItem*; 
        auto addCOB(hardware::COB* cob) -> view2d::COBItem*; 
        auto addNet(const QPointF& begin, const QPointF& end) -> view2d::NetItem*;

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
    };

}