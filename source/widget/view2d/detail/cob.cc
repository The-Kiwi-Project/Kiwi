#include "./cob.h"
#include "debug/debug.hh"
#include "hardware/cob/cob.hh"
#include "hardware/cob/cobdirection.hh"
#include "hardware/cob/cobunit.hh"
#include "qglobal.h"
#include "qline.h"
#include "qnamespace.h"
#include "qpainterpath.h"
#include "qpoint.h"
#include <QTextItem>
#include <cassert>
#include <ranges>
#include <QDebug>

namespace kiwi::widget::view2d {

    /*
    
                +--------------------------------------------+
                |                                            |
                +--------------------------------------------+
              (0, 0)
        +---+   +--------------------------------------------+  +---+ 
        |   |   |                                   +------+ |  |   |
        |   |   |                                   |      | |  |   |
        |   |   |                                   |      | |  |   |
        |   |   |                                   +------+ |  |   |
        |   |   |                           +------+         |  |   |
        |   |   |                           |      |         |  |   |
        |   |   |                           |      |         |  |   |
        |   |   |                           +------+         |  |   |
        |   |   |                        .                   |  |   |
        |   |   |                     .                      |  |   |
        |   |   |                  .                         |  |   |
        |   |   |         +------+                           |  |   |
        |   |   |         |      |                           |  |   |
        |   |   |         |      |                           |  |   |
        |   |   |         +------+                           |  |   |
        |   |   | +------+                                   |  |   |
        |   |   | |      |                                   |  |   |
        |   |   | |      |                                   |  |   |
        |   |   | +------+                                   |  |   |
        +---+   +--------------------------------------------+  +---+ 
    
                +--------------------------------------------+
                |                                            |
                +--------------------------------------------+

    */

    const QColor COBScene::BACK_COLOR = QColor(255,231,154);
    const QColor COBScene::COB_UNIT_COLOR = QColor(192,145,0);
    const QColor COBScene::TRAN_COLOR = Qt::black;

    COBScene::COBScene(hardware::COB* cob) : 
        QGraphicsScene{},
        _cob{cob}
    {
        this->setSceneRect(0, 0, 800, 800); // 场景大小
        this->addRect(
            QRectF{QPointF{}, QPointF{COB_WIDTH, COB_WIDTH}},
            QPen(), COBScene::BACK_COLOR
        );

        this->addRect(QRectF{this->leftTranLeftUpPosition(), QSizeF{TRAN_WIDTH, COB_WIDTH}}, QPen{}, TRAN_COLOR);
        this->addRect(QRectF{this->rightTranLeftUpPosition(), QSizeF{TRAN_WIDTH, COB_WIDTH}}, QPen{}, TRAN_COLOR);
        this->addRect(QRectF{this->upTranLeftUpPosition(), QSizeF{COB_WIDTH, TRAN_WIDTH}}, QPen{}, TRAN_COLOR);
        this->addRect(QRectF{this->downTranLeftUpPosition(), QSizeF{COB_WIDTH, TRAN_WIDTH}}, QPen{}, TRAN_COLOR);

        for (auto unit : std::ranges::views::iota(0, hardware::COB::UNIT_SIZE)) {
            auto leftup = QPointF{
                COB_UNIT_INTERVAL + (hardware::COB::UNIT_SIZE - 1 - unit) * (COB_UNIT_WIDTH + COB_UNIT_INTERVAL),
                COB_UNIT_INTERVAL + (COB_UNIT_WIDTH + COB_UNIT_INTERVAL) * unit
            };

            auto rect = QRectF{leftup, QSizeF{COB_UNIT_WIDTH, COB_UNIT_WIDTH}};
            this->addRect(rect, QPen(), COBScene::COB_UNIT_COLOR);

            for (auto unit_index : std::ranges::views::iota(0, hardware::COBUnit::WILTON_SIZE)) {
                auto cob_index = unit * hardware::COBUnit::WILTON_SIZE + unit_index;
                
            }

            // // 设置字体
            // QFont font("Arial", 16, QFont::Bold);
            // QFontMetrics metrics(font);

            // // 创建文字并计算居中位置
            // QString text = QString{"COB Unit %1"}.arg(unit);
            // int textWidth = metrics.horizontalAdvance(text);
            // int textHeight = metrics.height();
            // QPointF textPos(rect.center().x() - textWidth / 2., rect.center().y() - textHeight / 2.);

            // // 添加文字到场景
            // QGraphicsTextItem* textItem = this->addText(text, font);
            // textItem->setDefaultTextColor(Qt::blue);
            // textItem->setPos(textPos);

            // Line
            // Left
            // for (auto i : std::ranges::views::iota(0, hardware::COBUnit::WILTON_SIZE)) {
            //     auto y = rect.y() + INDEX_INTERVAL + i * INDEX_INTERVAL;
            //     auto x1 = -TRAN_INTERVAL;
            //     auto x2 = rect.x();
            //     this->addLine(x1, y, x2, y);
            // }

            // // Right
            // for (auto i : std::ranges::views::iota(0, hardware::COBUnit::WILTON_SIZE)) {
            //     auto y = rect.y() + INDEX_INTERVAL + i * INDEX_INTERVAL;
            //     auto x1 = rect.x() + COB_UNIT_WIDTH;
            //     auto x2 = COB_WIDTH + TRAN_INTERVAL;
            //     this->addLine(x1, y, x2, y);
            // }

            // // Up
            // for (auto i : std::ranges::views::iota(0, hardware::COBUnit::WILTON_SIZE)) {
            //     auto x = rect.x() + INDEX_INTERVAL + i * INDEX_INTERVAL;
            //     auto y1 = -TRAN_INTERVAL;
            //     auto y2 = rect.y();
            //     this->addLine(x, y1, x, y2);
            // }

            // // Up
            // for (auto i : std::ranges::views::iota(0, hardware::COBUnit::WILTON_SIZE)) {
            //     auto x = rect.x() + INDEX_INTERVAL + i * INDEX_INTERVAL;
            //     auto y1 = rect.y() + COB_UNIT_WIDTH;
            //     auto y2 = COB_WIDTH + TRAN_INTERVAL;
            //     this->addLine(x, y1, x, y2);
            // }
        }

        // Trans
        // Left
        for (auto trackIndex : std::ranges::views::iota(0, hardware::COB::INDEX_SIZE)) {
            auto trackPos = QPointF{COBScene::leftTranLeftUpPosition().x(), COBScene::horiIndexY(trackIndex)};
            auto cobIndex = hardware::COB::track_index_to_cob_index(trackIndex);

            auto unitIndex = cobIndex / hardware::COBUnit::WILTON_SIZE;
            auto innerIndex = cobIndex % hardware::COBUnit::WILTON_SIZE;

            auto cobY = COBScene::horiIndexY(unitIndex, innerIndex);
            auto cobPos = QPointF{COBScene::leftTranLeftUpPosition().x() + TRAN_WIDTH, cobY};
            auto cobUnitPos = QPointF{COBScene::cobuintLeftUpPosition(unitIndex).x(), cobY};

            // auto path = new HighLightPathItem{};
            auto path = QPainterPath{};
            path.moveTo(trackPos);
            path.lineTo(cobPos);
            path.lineTo(cobUnitPos);

            auto item = new HighLightPathItem{};
            item->setPath(path);
            this->addItem(item);
        }

        // Right
        for (auto trackIndex : std::ranges::views::iota(0, hardware::COB::INDEX_SIZE)) {
            auto trackPos = QPointF{COBScene::rightTranLeftUpPosition().x() + TRAN_WIDTH, COBScene::horiIndexY(trackIndex)};
            auto cobIndex = hardware::COB::track_index_to_cob_index(trackIndex);

            auto unitIndex = cobIndex / hardware::COBUnit::WILTON_SIZE;
            auto innerIndex = cobIndex % hardware::COBUnit::WILTON_SIZE;

            auto cobY = COBScene::horiIndexY(unitIndex, innerIndex);
            auto cobPos = QPointF{COBScene::rightTranLeftUpPosition().x(), cobY};
            auto cobUnitPos = QPointF{COBScene::cobuintLeftUpPosition(unitIndex).x() + COB_UNIT_WIDTH, cobY};

            // auto path = new HighLightPathItem{};
            auto path = QPainterPath{};
            path.moveTo(trackPos);
            path.lineTo(cobPos);
            path.lineTo(cobUnitPos);

            auto item = new HighLightPathItem{};
            item->setPath(path);
            this->addItem(item);
        }

        // Up
        for (auto trackIndex : std::ranges::views::iota(0, hardware::COB::INDEX_SIZE)) {
            // auto trackPos = QPointF{COBScene::rightTranLeftUpPosition().x() + TRAN_WIDTH, COBScene::horiIndexY(trackIndex)};
            auto trackPos = QPointF{COBScene::vertIndexX(trackIndex), COBScene::upTranLeftUpPosition().y()};
            auto cobIndex = hardware::COB::track_index_to_cob_index(trackIndex);

            auto unitIndex = cobIndex / hardware::COBUnit::WILTON_SIZE;
            auto innerIndex = cobIndex % hardware::COBUnit::WILTON_SIZE;

            auto cobX = COBScene::vertIndexX(unitIndex, innerIndex);
            auto cobPos = QPointF{cobX, COBScene::upTranLeftUpPosition().y() + TRAN_WIDTH};
            auto cobUnitPos = QPointF{cobX, COBScene::cobuintLeftUpPosition(unitIndex).y()};

            // auto path = new HighLightPathItem{};
            auto path = QPainterPath{};
            path.moveTo(trackPos);
            path.lineTo(cobPos);
            path.lineTo(cobUnitPos);

            auto item = new HighLightPathItem{};
            item->setPath(path);
            this->addItem(item);
        }

        // Down
        for (auto trackIndex : std::ranges::views::iota(0, hardware::COB::INDEX_SIZE)) {
            // auto trackPos = QPointF{COBScene::rightTranLeftUpPosition().x() + TRAN_WIDTH, COBScene::horiIndexY(trackIndex)};
            auto trackPos = QPointF{COBScene::vertIndexX(trackIndex), COBScene::downTranLeftUpPosition().y() + TRAN_WIDTH};
            auto cobIndex = hardware::COB::track_index_to_cob_index(trackIndex);

            auto unitIndex = cobIndex / hardware::COBUnit::WILTON_SIZE;
            auto innerIndex = cobIndex % hardware::COBUnit::WILTON_SIZE;

            auto cobX = COBScene::vertIndexX(unitIndex, innerIndex);
            auto cobPos = QPointF{cobX, COBScene::downTranLeftUpPosition().y()};
            auto cobUnitPos = QPointF{cobX, COBScene::cobuintLeftUpPosition(unitIndex).y() + COB_UNIT_WIDTH};

            // auto path = new HighLightPathItem{};
            auto path = QPainterPath{};
            path.moveTo(trackPos);
            path.lineTo(cobPos);
            path.lineTo(cobUnitPos);

            auto item = new HighLightPathItem{};
            item->setPath(path);
            this->addItem(item);
        }
        
        using enum hardware::COBDirection;
        using enum hardware::TrackDirection;
        using hardware::COBSwState;

        for (auto track_index = 0; track_index < hardware::COB::INDEX_SIZE; track_index++) {
            auto from_cob_index = hardware::COB::track_index_to_cob_index(track_index);
            auto to_cob_index = hardware::COB::cob_index_map(Left, from_cob_index, Up);
            auto value = this->_cob->get_sw_resgiter_value(Left, from_cob_index, Up);
            if (value == COBSwState::Connected) {
                auto p1 = COBScene::pinPosition(from_cob_index, Left);
                auto p2 = COBScene::pinPosition(to_cob_index, Up);
                this->addLine(QLineF{p1, p2});
            }
        }

        // Left to Down
        for (auto track_index = 0; track_index < hardware::COB::INDEX_SIZE; track_index++) {
            auto from_cob_index = hardware::COB::track_index_to_cob_index(track_index);
            auto to_cob_index = hardware::COB::cob_index_map(Left, from_cob_index, Down);
            auto value = this->_cob->get_sw_resgiter_value(Left, from_cob_index, Down);
            if (value == COBSwState::Connected) {
                auto p1 = COBScene::pinPosition(from_cob_index, Left);
                auto p2 = COBScene::pinPosition(to_cob_index, Down);
                this->addLine(QLineF{p1, p2});
            }
        }

        // Right to Up
        for (auto track_index = 0; track_index < hardware::COB::INDEX_SIZE; track_index++) {
            auto from_cob_index = hardware::COB::track_index_to_cob_index(track_index);
            auto to_cob_index = hardware::COB::cob_index_map(Right, from_cob_index, Up);
            auto value = this->_cob->get_sw_resgiter_value(Right, from_cob_index, Up);
            if (value == COBSwState::Connected) {
                auto p1 = COBScene::pinPosition(from_cob_index, Right);
                auto p2 = COBScene::pinPosition(to_cob_index, Up);
                this->addLine(QLineF{p1, p2});
            }
        }

        // Right to Down
        for (auto track_index = 0; track_index < hardware::COB::INDEX_SIZE; track_index++) {
            auto from_cob_index = hardware::COB::track_index_to_cob_index(track_index);
            auto to_cob_index = hardware::COB::cob_index_map(Right, from_cob_index, Down);
            auto value = this->_cob->get_sw_resgiter_value(Right, from_cob_index, Down);
            if (value == COBSwState::Connected) {
                auto p1 = COBScene::pinPosition(from_cob_index, Right);
                auto p2 = COBScene::pinPosition(to_cob_index, Down);
                this->addLine(QLineF{p1, p2});
            }
        }

        // Left to Right
        for (auto track_index = 0; track_index < hardware::COB::INDEX_SIZE; track_index++) {
            auto from_cob_index = hardware::COB::track_index_to_cob_index(track_index);
            auto to_cob_index = hardware::COB::cob_index_map(Left, from_cob_index, Right);
            auto value = this->_cob->get_sw_resgiter_value(Left, from_cob_index, Right);
            if (value == COBSwState::Connected) {
                auto p1 = COBScene::pinPosition(from_cob_index, Left);
                auto p2 = COBScene::pinPosition(to_cob_index, Right);
                this->addLine(QLineF{p1, p2});
            }
        }

        // Up to Down
        for (auto track_index = 0; track_index < hardware::COB::INDEX_SIZE; track_index++) {
            auto from_cob_index = hardware::COB::track_index_to_cob_index(track_index);
            auto to_cob_index = hardware::COB::cob_index_map(Up, from_cob_index, Down);
            auto value = this->_cob->get_sw_resgiter_value(Up, from_cob_index, Down);
            if (value == COBSwState::Connected) {
                auto p1 = COBScene::pinPosition(from_cob_index, Up);
                auto p2 = COBScene::pinPosition(to_cob_index, Down);
                this->addLine(QLineF{p1, p2});
            }
        }
    }

    auto COBScene::cobuintLeftUpPosition(int uintIndex) -> QPointF {
        return QPointF{
            COB_UNIT_INTERVAL + (hardware::COB::UNIT_SIZE - 1 - uintIndex) * (COB_UNIT_WIDTH + COB_UNIT_INTERVAL),
            COB_UNIT_INTERVAL + (COB_UNIT_WIDTH + COB_UNIT_INTERVAL) * uintIndex
        };
    }

    auto COBScene::horiIndexY(int index) -> qreal {
        assert(index >= 0 && index < hardware::COB::INDEX_SIZE);
        auto unitIndex = index / hardware::COBUnit::WILTON_SIZE;
        auto innerIndex = index % hardware::COBUnit::WILTON_SIZE;
        return COBScene::horiIndexY(unitIndex, innerIndex);
    }

    auto COBScene::vertIndexX(int index) -> qreal {
        assert(index >= 0 && index < hardware::COB::INDEX_SIZE);
        auto uintIndex = index / hardware::COBUnit::WILTON_SIZE;
        auto innerIndex = index % hardware::COBUnit::WILTON_SIZE;
        return COBScene::vertIndexX(uintIndex, innerIndex);
    }

    auto COBScene::horiIndexY(int unitIndex, int innerIndex) -> qreal {
        assert(unitIndex >= 0 && unitIndex < hardware::COB::UNIT_SIZE);
        assert(innerIndex >= 0 && innerIndex < hardware::COBUnit::WILTON_SIZE);
        return COBScene::cobuintLeftUpPosition(unitIndex).y() + INDEX_INTERVAL * (innerIndex + 1);
    }

    auto COBScene::vertIndexX(int unitIndex, int innerIndex) -> qreal {
        assert(unitIndex >= 0 && unitIndex < hardware::COB::UNIT_SIZE);
        assert(innerIndex >= 0 && innerIndex < hardware::COBUnit::WILTON_SIZE);
        return COBScene::cobuintLeftUpPosition(unitIndex).x() + COB_UNIT_WIDTH - INDEX_INTERVAL * (innerIndex + 1);
    }

    auto COBScene::pinPosition(int unitIndex, int innerIndex, hardware::COBDirection dir) -> QPointF {
        auto leftPos = COBScene::cobuintLeftUpPosition(unitIndex);
        switch (dir) {
            case hardware::COBDirection::Left: {
                return QPointF {leftPos.x(), leftPos.y() + INDEX_INTERVAL * (innerIndex + 1)};
            }
            case hardware::COBDirection::Right: {
                return QPointF {leftPos.x() + COB_UNIT_WIDTH, leftPos.y() + INDEX_INTERVAL * (innerIndex + 1)};
            }
            case hardware::COBDirection::Up: {
                return QPointF {leftPos.x() + COB_UNIT_WIDTH - INDEX_INTERVAL * (innerIndex + 1), leftPos.y()};
            }
            case hardware::COBDirection::Down: {
                return QPointF {leftPos.x() + COB_UNIT_WIDTH - INDEX_INTERVAL * (innerIndex + 1), leftPos.y() + COB_UNIT_WIDTH};
            }
        }

        debug::unreachable();
    }

    auto COBScene::pinPosition(int index, hardware::COBDirection dir) -> QPointF {
        assert(index >= 0 && index < hardware::COB::INDEX_SIZE);
        auto unitIndex = index / hardware::COBUnit::WILTON_SIZE;
        auto innerIndex = index % hardware::COBUnit::WILTON_SIZE;
        return COBScene::pinPosition(unitIndex, innerIndex, dir);
    }

    auto COBScene::leftTranLeftUpPosition() -> QPointF {
        return QPointF{-TRAN_INTERVAL - TRAN_WIDTH, 0};
    }

    auto COBScene::rightTranLeftUpPosition() -> QPointF {
        return QPointF{TRAN_INTERVAL + COB_WIDTH, 0};
    }

    auto COBScene::upTranLeftUpPosition() -> QPointF {
        return QPointF{0., -TRAN_INTERVAL - TRAN_WIDTH};
    }

    auto COBScene::downTranLeftUpPosition() -> QPointF {
        return QPointF{0, TRAN_INTERVAL + COB_WIDTH};
    }

}