#include "./view2dwidget.h"
#include "qcolor.h"
#include "qgraphicsitem.h"
#include "qnamespace.h"
#include "qobject.h"
#include "qpoint.h"
#include "./item/cobitem.hh"
#include <QScrollBar>

#include "widget/view/2d/item/chip.h"
#include "widget/view/2d/item/item.hh"
#include "widget/view/2d/item/tobitem.hh"
#include "widget/view/2d/item/topdieitem.hh"
#include <QPainter>
#include <QBrush>
#include <hardware/interposer.hh>
#include <circuit/basedie.hh>
#include <QDebug>
#include <cmath>
#include <memory>

namespace kiwi::widget {

    View2DWidget::View2DWidget(
        hardware::Interposer* interposer, 
        circuit::BaseDie* basedie,
        QWidget *parent
    ) :
        QGraphicsView{parent},
        _interposer{interposer},
        _basedie{basedie}
    {
        this->_scene = new QGraphicsScene{};
        this->setScene(this->_scene);
        this->setDragMode(QGraphicsView::RubberBandDrag);
        this->setInteractive(true);
        this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse); // 缩放以鼠标为中心

        // this->_scene->setSceneRect(-500, -500, 1000, 1000);

        // auto item = new QGraphicsRectItem{50., 50., 10., 10.};
        // this->_scene->addItem(item);

        QGraphicsItem *c = new Chip(QColor::fromRgb(10, 255, 10), 0, 0);
        this->_scene->addItem(c);

        // this->scale(1, -1);
        // this->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
        
        this->_scene->addItem(new COBItem{QPoint{0, 0}});
        this->_scene->addItem(new TOBItem{QPoint{100, 100}});

        
        auto cobArrayHeight = static_cast<float>(hardware::Interposer::COB_ARRAY_HEIGHT);
        auto cobArrayWidth  = static_cast<float>(hardware::Interposer::COB_ARRAY_WIDTH);


        // this->addCOBItems();
        // this->addTOBItems();
        // this->addTopDieInstItems();
    }

    View2DWidget::~View2DWidget() noexcept {}

    void View2DWidget::addCOBItems() {
        for (const auto& [coord, _] : this->_interposer->cobs()) {
            auto offset = this->cobPosition(coord);
            this->_items.emplace_back(std::make_unique<COBItem>(offset));
        }
    }

    auto View2DWidget::cobPosition(const hardware::COBCoord& coord) -> QPoint {
        return QPoint{
            static_cast<int>(coord.col) * COB_PITCH, 
            static_cast<int>(coord.row) * COB_PITCH
        };
    }

    void View2DWidget::addTOBItems() {
        for (const auto& [coord, _] : this->_interposer->tobs()) {
            auto offset = this->tobPosition(coord);
            this->_items.emplace_back(std::make_unique<TOBItem>(offset));
        }
    }

    auto View2DWidget::tobPosition(const hardware::TOBCoord& coord) -> QPoint {
        auto pos = this->cobPosition(hardware::Interposer::TOB_COORD_MAP.at(coord));
        pos.setY(pos.y() - (COB_PITCH / 2));
        return pos;
    }

    void View2DWidget::addTopDieInstItems() {
        for (const auto& [name, topdie] : this->_basedie->topdie_insts()) {
            auto offset = this->topDieInstPosition(topdie.tob()->coord());
            this->_items.emplace_back(std::make_unique<TopDieInstItem>(offset));
        }
    }

    auto View2DWidget::topDieInstPosition(const hardware::TOBCoord& coord) -> QPoint{
        return this->tobPosition(coord);
    }

    auto View2DWidget::trackPosition(const hardware::TrackCoord& coord) -> std::Tuple<QPoint, QPoint> {
        auto cobPosition = this->cobPosition(coord);
        // auto 
    }

    void View2DWidget::wheelEvent(QWheelEvent *event) {
        const double scaleFactor = 1.15;
        if (event->angleDelta().y() > 0) {
            scale(scaleFactor, scaleFactor); // 放大
        } else {
            scale(1.0 / scaleFactor, 1.0 / scaleFactor); // 缩小
        }
    }

}