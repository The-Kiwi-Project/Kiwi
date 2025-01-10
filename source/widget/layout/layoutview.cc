#include "./layoutview.h"
#include "./layoutscene.h"

#include "./item/topdieinstitem.h"
#include "circuit/connection/connection.hh"
#include "hardware/tob/tobcoord.hh"
#include "qbrush.h"
#include "qcolor.h"
#include "qglobal.h"
#include "qgraphicsitem.h"
#include "qobject.h"
#include "qpen.h"
#include "qpoint.h"
#include "./item/netitem.h"
#include "./item/tobitem.h"
#include "std/utility.hh"
#include "widget/layout/item/pinitem.h"
#include "circuit/topdieinst/topdieinst.hh"

#include <QPainter>
#include <QBrush>
#include <cassert>
#include <hardware/interposer.hh>
#include <hardware/tob/tob.hh>
#include <circuit/basedie.hh>
#include <QDebug>
#include <QWheelEvent>
#include <QScrollBar>
#include <cmath>


namespace kiwi::widget {

    using namespace layout;

    const QColor LayoutView::BACK_COLOR = QColor::fromRgb(100, 200, 100);

    LayoutView::LayoutView(
        hardware::Interposer* interposer, 
        circuit::BaseDie* basedie,
        QWidget *parent
    ) :
        GraphicsView{parent},
        _interposer{interposer},
        _basedie{basedie}
    {
        this->_scene = new LayoutScene {};
        this->setScene(this->_scene);
        this->setDragMode(QGraphicsView::RubberBandDrag);
        this->setInteractive(true);
        this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

        // auto i = new TopDieInstItem {nullptr};
        // this->_scene->addItem(i);

        // i = new TopDieInstItem {nullptr};
        // this->_scene->addItem(i);
        // i->setPos(1000, 1000);

        auto t = new TOBItem {nullptr};
        this->_scene->addItem(t);

        // t->setPos(500, 500);

        // t = new TOBItem {nullptr};
        // this->_scene->addItem(t);

        // t->setPos(1200, 500);

        auto maxRow = std::i64{-1};
        auto maxCol = std::i64{-1};
        for (auto& [coord, tob] : this->_interposer->tobs()) {
            auto t = this->_scene->addTOB(tob.get());
            t->setPos(LayoutView::tobPosition(coord));
            maxRow = qMax(maxRow, coord.row);
            maxCol = qMax(maxCol, coord.col);
        }

        auto r = new QGraphicsRectItem {QRectF{
            LayoutView::tobPosition(hardware::TOBCoord{0, 0}) - QPointF{TOBItem::WIDTH/2 + TOB_INTERVAL, -TOBItem::HEIGHT/2 - TOB_INTERVAL},
            LayoutView::tobPosition(hardware::TOBCoord{maxRow, maxCol}) + QPointF{TOBItem::WIDTH/2 + TOB_INTERVAL, -TOBItem::HEIGHT/2 - TOB_INTERVAL}
        }};
        r->setBrush(Qt::gray);
        r->setZValue(-5);
        this->_scene->addItem(r);

        for (auto& [name, inst] : this->_basedie->topdie_insts()) {
            auto i = this->_scene->addTopDieInst(inst.get());
            if (inst->tob() != nullptr) {
                i->setPos(LayoutView::tobPosition(inst->tob()->coord()));
            } else {
                i->setPos(LayoutView::tobPosition(hardware::TOBCoord{-1, -1}));
            }
        }

        // for (auto& [sync, connections] : this->_basedie->connections()) {
        //     for (const auto& connection : connections) {
        //         layout::PinItem* beginPin, *endPin;
        //         bool cont = false;

        //         std::match(connection->input_pin().connected_point(),
        //             [this, &cont](const circuit::ConnectExPort& eport) {
        //                 cont = true;
        //             },
        //             [this, &beginPin](const circuit::ConnectBump& bump) {
        //                 auto inst = this->_scene->topdieinstMap().value(bump.inst);
        //                 auto pin = inst->pins()[bump.inst->topdie()->pins_map().at(bump.name)];
        //                 beginPin = pin;
        //             }
        //         );

        //         std::match(connection->output().connected_point(),
        //             [this, &cont](const circuit::ConnectExPort& eport) {
        //                 cont = true;
        //             },
        //             [this, &endPin](const circuit::ConnectBump& bump) {
        //                 auto inst = this->_scene->topdieinstMap().value(bump.inst);
        //                 auto pin = inst->pins()[bump.inst->topdie()->pins_map().at(bump.name)];
        //                 endPin = pin;
        //             }
        //         );

        //         if (cont) {
        //             continue;
        //         }

        //         this->_scene->addNet(beginPin, endPin);
        //     }
        // }
    }

    LayoutView::~LayoutView() noexcept {}

    auto LayoutView::tobPosition(const hardware::TOBCoord& coord) -> QPointF {
        return QPointF{coord.col * (TOBItem::WIDTH + TOB_INTERVAL), -coord.row * (TOBItem::HEIGHT + TOB_INTERVAL)};
    }

}