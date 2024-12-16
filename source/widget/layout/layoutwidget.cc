#include "./layoutview.h"

#include "./item/topdieinstitem.h"
#include "widget/layout/item/tobitem.h"

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

    using namespace layout;

    LayoutView::LayoutView(
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
        this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

        auto i = new TopDieInstItem {nullptr};
        this->_scene->addItem(i);

        i = new TopDieInstItem {nullptr};
        this->_scene->addItem(i);
        i->setPos(1000, 1000);

        auto t = new TOBItem {nullptr};
        this->_scene->addItem(t);

        t->setPos(500, 500);

        t = new TOBItem {nullptr};
        this->_scene->addItem(t);

        t->setPos(1200, 500);
    }

    LayoutView::~LayoutView() noexcept {}

    void LayoutView::wheelEvent(QWheelEvent *event) {
        const double scaleFactor = 1.15;
        if (event->angleDelta().y() > 0) {
            scale(scaleFactor, scaleFactor); // 放大
        } else {
            scale(1.0 / scaleFactor, 1.0 / scaleFactor); // 缩小
        }
    }

}