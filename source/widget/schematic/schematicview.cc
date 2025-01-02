#include "./schematicview.h"

#include "./item/pinitem.h"
#include "./item/topdieinstitem.h"
#include "./item/exportitem.hh"
#include "widget/schematic/schematicscene.h"
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

    using namespace schematic;

    SchematicView::SchematicView(
        hardware::Interposer* interposer, 
        circuit::BaseDie* basedie,
        QWidget *parent
    ) :
        QGraphicsView{parent},
        _interposer{interposer},
        _basedie{basedie}
    {
        this->_scene = new SchematicScene {};
        this->setScene(this->_scene);
        this->setDragMode(QGraphicsView::RubberBandDrag);
        this->setInteractive(true);
        this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

        for (auto& [name, topdie] : this->_basedie->topdie_insts()) {
            auto t = new TopDieInstanceItem{topdie.get(), this->_scene};
            this->_topdieInstItems.push_back(t);
            this->_scene->addItem(t);
        }

        auto spacing = 20.0;

        int cols = std::ceil(std::sqrt(this->_topdieInstItems.size()));
        int rows = std::ceil(static_cast<double>(this->_topdieInstItems.size()) / cols);

        int startX = spacing;
        int startY = spacing;

        for (size_t i = 0; i < this->_topdieInstItems.size(); ++i) {
            int row = i / cols;
            int col = i % cols;

            int x = startX + col * (this->_topdieInstItems[i]->width() + spacing);
            int y = startY + row * (this->_topdieInstItems[i]->width() + spacing);

            this->_topdieInstItems[i]->setPos(x, y);
        }

        auto chip1 = this->_topdieInstItems[0];
        auto chip2 = this->_topdieInstItems[6];
    
        auto p1 = chip1->pinitems().begin().value();
        auto p2 = chip2->pinitems().begin().value();

        // auto net = p1->connectTo(p2);
        // this->_scene->addItem(net);
        this->_scene->connectPins(p1, p2);

        auto p = this->_scene->addExPort("xinzhai_outout_0");
        p->setPos(-100, 500);
    }

    SchematicView::~SchematicView() noexcept {}

    void SchematicView::wheelEvent(QWheelEvent *event) {
        const double scaleFactor = 1.15;
        if (event->angleDelta().y() > 0) {
            scale(scaleFactor, scaleFactor); // 放大
        } else {
            scale(1.0 / scaleFactor, 1.0 / scaleFactor); // 缩小
        }
    }

}