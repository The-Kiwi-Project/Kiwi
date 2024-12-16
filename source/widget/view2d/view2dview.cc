#include "./view2dview.h"
#include "qgraphicsitem.h"
#include "qgraphicsscene.h"
#include "qobject.h"

#include "./item/cobitem.h"
#include "./item/tobitem.h"
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
        QGraphicsView{parent},
        _interposer{interposer},
        _basedie{basedie}
    {
        this->_scene = new QGraphicsScene{};
        this->setScene(this->_scene);
        this->setDragMode(QGraphicsView::RubberBandDrag);
        this->setInteractive(true);
        this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

        for (int r = 0; r < 8; r++) {
            for (int c = 0; c < 12; c++) {
                auto cobitem = new COBItem {nullptr};
                cobitem->setPos(c * 300, r * 400);
                this->_scene->addItem(cobitem);
            }
        }

        for (int r = 0; r < 4; r++) {
            for (int c = 0; c < 4; c++) {
                auto tobitem = new TOBItem {nullptr};
                tobitem->setPos(3* c * 300, (2 * r + 1 ) * 400 - 200);
                this->_scene->addItem(tobitem);
            }
        }

        auto cobArrayHeight = static_cast<float>(hardware::Interposer::COB_ARRAY_HEIGHT);
        auto cobArrayWidth  = static_cast<float>(hardware::Interposer::COB_ARRAY_WIDTH);
    }

    View2DView::~View2DView() noexcept {}

    void View2DView::wheelEvent(QWheelEvent *event) {
        const double scaleFactor = 1.15;
        if (event->angleDelta().y() > 0) {
            scale(scaleFactor, scaleFactor); // 放大
        } else {
            scale(1.0 / scaleFactor, 1.0 / scaleFactor); // 缩小
        }
    }

}