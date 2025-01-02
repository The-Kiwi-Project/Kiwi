#include "./view2dview.h"
#include "debug/debug.hh"
#include "hardware/cob/cob.hh"
#include "hardware/track/trackcoord.hh"
#include "qglobal.h"
#include "qgraphicsitem.h"
#include "qgraphicsscene.h"
#include "qline.h"
#include "qobject.h"

#include "./item/cobitem.h"
#include "./item/tobitem.h"
#include "qpoint.h"
#include "std/utility.hh"
#include <QPainter>
#include <QBrush>
#include <cassert>
#include <hardware/interposer.hh>
#include <circuit/basedie.hh>
#include <QDebug>
#include <QWheelEvent>
#include <QScrollBar>
#include <cmath>

namespace kiwi::widget {
    using namespace view2d;

    View2DView::View2DView(
        hardware::Interposer* interposer, 
        circuit::BaseDie* basedie,
        QWidget *parent
    ) :
        GraphicsView{parent},
        _interposer{interposer},
        _basedie{basedie}
    {
        this->_scene = new View2DScene{};
        this->setScene(this->_scene);
        this->setDragMode(QGraphicsView::RubberBandDrag);
        this->setInteractive(true);
        this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

        for (const auto& [coord, cob] : this->_interposer->cobs()) {
            auto pos = View2DView::cobPosition(coord);
            auto item = this->_scene->addCOB(cob.get());
            item->setPos(pos);
        }

        for (const auto& [coord, tob] : this->_interposer->tobs()) {
            auto pos = View2DView::tobPosition(tob->coord_in_interposer());
            auto item = this->_scene->addTOB(tob.get());
            item->setPos(pos);
        }

        this->displayCOBConnections();
    }

    View2DView::~View2DView() noexcept {}

    auto View2DView::displayCOBConnections() -> void {
        using enum hardware::COBDirection;
        using enum hardware::TrackDirection;
        using hardware::COBSwState;
        for (auto& [coord, cob] : this->_interposer->cobs()) {
            // Left to Up
            for (auto track_index = 0; track_index < hardware::COB::INDEX_SIZE; track_index++) {
                auto from_cob_index = hardware::COB::track_index_to_cob_index(track_index);
                auto to_cob_index = hardware::COB::cob_index_map(Left, from_cob_index, Up);
                auto value = cob->get_sw_resgiter_value(Left, from_cob_index, Up);
                if (value == COBSwState::Connected) {
                    auto [beginl, endl] = this->trackPositions(cob->to_dir_track_coord(Left, from_cob_index));
                    auto [beginu, endu] = this->trackPositions(cob->to_dir_track_coord(Up, to_cob_index));

                    /*
                                           endu
                                            |
                                          beginu
                                       +---------
                        beginl -> endl |
                                       |

                    */

                    this->addTrack(beginl, endl);
                    this->addTrack(beginu, endu);
                    this->addTrack(endl, beginu);
                }
            }

            // Left to Down
            for (auto track_index = 0; track_index < hardware::COB::INDEX_SIZE; track_index++) {
                auto from_cob_index = hardware::COB::track_index_to_cob_index(track_index);
                auto to_cob_index = hardware::COB::cob_index_map(Left, from_cob_index, Down);
                auto value = cob->get_sw_resgiter_value(Left, from_cob_index, Down);
                if (value == COBSwState::Connected) {
                    auto [beginl, endl] = this->trackPositions(cob->to_dir_track_coord(Left, from_cob_index));
                    auto [begind, endd] = this->trackPositions(cob->to_dir_track_coord(Down, to_cob_index));

                    /*
                                       |
                        beginl -> endl |
                                       |
                                       +------------
                                          endd
                                           |
                                         begind 
                    */

                    this->addTrack(beginl, endl);
                    this->addTrack(begind, endd);
                    this->addTrack(endl, endd);
                }
            }

            // Right to Up
            for (auto track_index = 0; track_index < hardware::COB::INDEX_SIZE; track_index++) {
                auto from_cob_index = hardware::COB::track_index_to_cob_index(track_index);
                auto to_cob_index = hardware::COB::cob_index_map(Right, from_cob_index, Up);
                auto value = cob->get_sw_resgiter_value(Right, from_cob_index, Up);
                if (value == COBSwState::Connected) {
                    auto [beginr, endr] = this->trackPositions(cob->to_dir_track_coord(Right, from_cob_index));
                    auto [beginu, endu] = this->trackPositions(cob->to_dir_track_coord(Up, to_cob_index));

                    /*
                                endu
                                  |
                                beginu
                            ---------+
                                     |
                                     | beginr -> endr
                                     |
                    */

                    this->addTrack(beginr, endr);
                    this->addTrack(beginu, endu);
                    this->addTrack(beginr, beginu);
                }
            }

            // Right to Down
            for (auto track_index = 0; track_index < hardware::COB::INDEX_SIZE; track_index++) {
                auto from_cob_index = hardware::COB::track_index_to_cob_index(track_index);
                auto to_cob_index = hardware::COB::cob_index_map(Right, from_cob_index, Down);
                auto value = cob->get_sw_resgiter_value(Right, from_cob_index, Down);
                if (value == COBSwState::Connected) {
                    auto [beginr, endr] = this->trackPositions(cob->to_dir_track_coord(Right, from_cob_index));
                    auto [begind, endd] = this->trackPositions(cob->to_dir_track_coord(Down, to_cob_index));

                    /*
                                        |
                                        | beginr -> endr
                                        |
                            ------------+
                                 endd
                                   |
                                begind 
                    */

                    this->addTrack(beginr, endr);
                    this->addTrack(begind, endd);
                    this->addTrack(beginr, endd);
                }
            }

            // Left to Right
            for (auto track_index = 0; track_index < hardware::COB::INDEX_SIZE; track_index++) {
                auto from_cob_index = hardware::COB::track_index_to_cob_index(track_index);
                auto to_cob_index = hardware::COB::cob_index_map(Left, from_cob_index, Right);
                auto value = cob->get_sw_resgiter_value(Left, from_cob_index, Right);
                if (value == COBSwState::Connected) {
                    auto [beginl, endl] = this->trackPositions(cob->to_dir_track_coord(Left, from_cob_index));
                    auto [beginr, endr] = this->trackPositions(cob->to_dir_track_coord(Right, to_cob_index));

                    /*
                                       |        |
                        beginl -> endl |        | beginr -> endr
                                       |        |

                    */

                    this->addTrack(beginl, endl);
                    this->addTrack(beginr, endr);
                    this->addTrack(endl, beginr);
                }
            }

            // Up to Down
            for (auto track_index = 0; track_index < hardware::COB::INDEX_SIZE; track_index++) {
                auto from_cob_index = hardware::COB::track_index_to_cob_index(track_index);
                auto to_cob_index = hardware::COB::cob_index_map(Up, from_cob_index, Down);
                auto value = cob->get_sw_resgiter_value(Up, from_cob_index, Down);
                if (value == COBSwState::Connected) {
                    auto [beginu, endu] = this->trackPositions(cob->to_dir_track_coord(Up, from_cob_index));
                    auto [begind, endd] = this->trackPositions(cob->to_dir_track_coord(Down, to_cob_index));

                    /*
                                endu
                                  |
                                beginu
                            ------------


                            ------------
                                 endd
                                   |
                                begind 
                    */

                    this->addTrack(beginu, endu);
                    this->addTrack(begind, endd);
                    this->addTrack(beginu, endd);
                }
            }
        }
    }

    void View2DView::addTrack(const hardware::TrackCoord& coord) {
        debug::debug_fmt("Add track in '{}'", coord);
        auto [p1, p2] = View2DView::trackPositions(coord);
        this->addTrack(p1, p2);
    }

    void View2DView::addTrack(const QPointF& begin, const QPointF& end) {
        this->_scene->addLine(QLineF{begin, end});
    }

    auto View2DView::trackPositions(const hardware::TrackCoord& coord) -> std::Tuple<QPointF, QPointF> {
        switch (coord.dir) {
            case hardware::TrackDirection::Horizontal: {
                /*
                
                    left cob  +------+  right cob

                */

                auto leftCOBPos = View2DView::cobPosition({coord.row, coord.col - 1});
                auto rightCOBPos = View2DView::cobPosition({coord.row, coord.col});

                assert(leftCOBPos.y() == rightCOBPos.y());

                auto y = 
                    leftCOBPos.y() + 
                    view2d::COBItem::TRANFORM_LENGTH / 2 - 
                    coord.index * ((qreal)COBItem::TRANFORM_LENGTH / (qreal)hardware::COB::INDEX_SIZE);

                auto leftX = leftCOBPos.x() + (COBItem::CORE_WIDTH + COBItem::TRANFORM_WIDTH) / 2.;
                auto righX = rightCOBPos.x() - (COBItem::CORE_WIDTH + COBItem::TRANFORM_WIDTH) / 2.;

                return std::Tuple<QPointF, QPointF>{QPointF{leftX, y}, QPointF{righX, y}};
            }
            case hardware::TrackDirection::Vertical: {
                /*
                        up cob
                          +
                          |
                          +
                        down cob
                */
                auto upCOBPos = View2DView::cobPosition({coord.row, coord.col});
                auto downCOBPos = View2DView::cobPosition({coord.row - 1, coord.col});

                assert(upCOBPos.x() == downCOBPos.x());

                auto x = 
                    upCOBPos.x() -
                    view2d::COBItem::TRANFORM_LENGTH / 2 +
                    coord.index * ((qreal)COBItem::TRANFORM_LENGTH / (qreal)hardware::COB::INDEX_SIZE);

                auto upY = upCOBPos.y() + (COBItem::CORE_WIDTH + COBItem::TRANFORM_WIDTH) / 2.;
                auto downY = downCOBPos.y() - (COBItem::CORE_WIDTH + COBItem::TRANFORM_WIDTH) / 2.;

                return std::Tuple<QPointF, QPointF>{QPointF{x, upY}, QPointF{x, downY}};
            }
        }
        debug::unreachable();
    }

    auto View2DView::cobPosition(const hardware::COBCoord& coord) -> QPointF {
        return QPointF {
            coord.col * (view2d::COBItem::CORE_WIDTH + COB_WIDTH_INTERVAL), 
            -coord.row * (view2d::COBItem::CORE_WIDTH + COB_HEIGHT_INTERVAL)
        };
    }

    auto View2DView::tobPosition(const hardware::COBCoord& coord) -> QPointF {
        return QPointF {
            coord.col * (view2d::COBItem::CORE_WIDTH + COB_WIDTH_INTERVAL), 
            -(coord.row + 0.5) * (view2d::COBItem::CORE_WIDTH + COB_HEIGHT_INTERVAL)
        };
    }

}