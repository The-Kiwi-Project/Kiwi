#pragma once

#include "./highlightpathitem.h"
#include "hardware/cob/cobdirection.hh"
#include "qpoint.h"
#include <hardware/cob/cob.hh>

namespace kiwi::widget::view2d {

    class COBScene : public QGraphicsScene {
        static constexpr qreal COB_UNIT_WIDTH = 180;
        static constexpr qreal COB_UNIT_INTERVAL = 10;
        static constexpr qreal COB_WIDTH = 
            (qreal)hardware::COB::UNIT_SIZE * COB_UNIT_WIDTH + 
            (hardware::COB::UNIT_SIZE + 1) * COB_UNIT_INTERVAL;

        static constexpr qreal TRAN_INTERVAL = 50;
        static constexpr qreal TRAN_WIDTH = 80;

        static constexpr qreal INDEX_INTERVAL = COB_UNIT_WIDTH / (hardware::COBUnit::WILTON_SIZE + 1);

        static const QColor BACK_COLOR;
        static const QColor COB_UNIT_COLOR;
        static const QColor TRAN_COLOR;

    public:
        COBScene(hardware::COB* cob);
    
    private:
        static auto cobuintLeftUpPosition(int index) -> QPointF;

        static auto horiIndexY(int unitIndex, int innerIndex) -> qreal;
        static auto vertIndexX(int unitIndex, int innerIndex) -> qreal;

        static auto horiIndexY(int index) -> qreal;
        static auto vertIndexX(int index) -> qreal;

        static auto leftTranLeftUpPosition() -> QPointF;
        static auto rightTranLeftUpPosition() -> QPointF;
        static auto upTranLeftUpPosition() -> QPointF;
        static auto downTranLeftUpPosition() -> QPointF;

        static auto pinPosition(int unitIndex, int innerIndex, hardware::COBDirection dir) -> QPointF;
        static auto pinPosition(int index, hardware::COBDirection dir) -> QPointF;

    private:
        hardware::COB* _cob{};
    };

}