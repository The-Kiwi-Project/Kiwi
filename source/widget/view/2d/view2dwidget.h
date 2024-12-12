#pragma once

#include "hardware/cob/cob.hh"
#include "hardware/cob/cobcoord.hh"
#include "hardware/tob/tobcoord.hh"
#include "hardware/track/trackcoord.hh"
#include "qevent.h"
#include "qpoint.h"
#include "qregion.h"
#include "std/memory.hh"
#include "std/utility.hh"
#include "widget/view/2d/item/cobitem.hh"
#include "widget/view/2d/item/item.hh"
#include "widget/view/2d/item/topdieitem.hh"
#include <QWidget>
#include <QGraphicsView>
#include <std/collection.hh>

namespace kiwi::hardware {
    class Interposer;
    class TOB;
    class COB;
    class Track;
};

namespace kiwi::circuit {
    class BaseDie;
    class TopDieInstance;
};

namespace kiwi::widget {
   
    class View2DWidget : public QGraphicsView {

        enum {
            COB_INTERVAL = 50,
            COB_PITCH = COB_INTERVAL + COBItem::WIDTH
        };

    public:
        explicit View2DWidget(
            hardware::Interposer* interposer, 
            circuit::BaseDie* basedie,
            QWidget *parent = nullptr);

        ~View2DWidget() noexcept;

    protected:
        void addCOBItems();
        auto cobPosition(const hardware::COBCoord& coord) -> QPoint;

        void addTOBItems();
        auto tobPosition(const hardware::TOBCoord& coord) -> QPoint;

        void addTopDieInstItems();
        auto topDieInstPosition(const hardware::TOBCoord& coord) -> QPoint;

        auto trackPosition(const hardware::TrackCoord& coord) -> std::Tuple<QPoint, QPoint>;

    protected:
        virtual void wheelEvent(QWheelEvent *event) override;

    protected:
        hardware::Interposer* _interposer {nullptr};
        circuit::BaseDie* _basedie {nullptr};

        std::Vector<std::Box<Item>> _items;
        std::Vector<TopDieInstItem*> _topdieinst_items;
        QRect interposerBoard;

        std::Vector<hardware::TOB*> _tobs {};
        std::Vector<hardware::COB*> _cobs {};
        std::Vector<circuit::TopDieInstance*> _topdieinsts {};

        // view
        QGraphicsScene* _scene;
        
        qreal _offsetX {0.};
        qreal _offsetY {0.};
        qreal _scale {1.0};
        Qt::MouseButton _mouseButton {Qt::NoButton};
        bool _isDragging = false;
        QPoint _lastMousePos {0, 0};
    };

}