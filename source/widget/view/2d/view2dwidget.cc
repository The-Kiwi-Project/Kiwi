#include "./view2dwidget.h"
#include "qgraphicsitem.h"
#include "qgraphicsscene.h"
#include "qobject.h"

#include "./item/topdieinstitem.h"
#include <QPainter>
#include <QBrush>
#include <hardware/interposer.hh>
#include <circuit/basedie.hh>
#include <QDebug>
#include <QWheelEvent>
#include <QScrollBar>
#include <cmath>

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
        this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

        for (auto& [name, topdie] : this->_basedie->topdie_insts()) {
            QGraphicsItem* t = new TopDieInstanceItem{&topdie};
            this->_scene->addItem(t);
        }
                
        auto cobArrayHeight = static_cast<float>(hardware::Interposer::COB_ARRAY_HEIGHT);
        auto cobArrayWidth  = static_cast<float>(hardware::Interposer::COB_ARRAY_WIDTH);
    }

    View2DWidget::~View2DWidget() noexcept {}

    void View2DWidget::wheelEvent(QWheelEvent *event) {
        const double scaleFactor = 1.15;
        if (event->angleDelta().y() > 0) {
            scale(scaleFactor, scaleFactor); // 放大
        } else {
            scale(1.0 / scaleFactor, 1.0 / scaleFactor); // 缩小
        }
    }

}