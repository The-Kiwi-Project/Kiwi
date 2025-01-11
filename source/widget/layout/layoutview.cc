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
        this->_scene = new LayoutScene {this->_basedie, this->_interposer};
        this->setScene(this->_scene);
        this->setDragMode(QGraphicsView::RubberBandDrag);
        this->setInteractive(true);
        this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        this->resize(1000, 800);
    }

    LayoutView::~LayoutView() noexcept {}

}