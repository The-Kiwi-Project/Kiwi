#pragma once

#include "hardware/cob/cob.hh"
#include "qpoint.h"
#include "qregion.h"
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
        };

    public:
        explicit View2DWidget(
            hardware::Interposer* interposer, 
            circuit::BaseDie* basedie,
            QWidget *parent = nullptr);

        ~View2DWidget() noexcept;

    protected:
        virtual void wheelEvent(QWheelEvent *event) override;

    protected:
        hardware::Interposer* _interposer {nullptr};
        circuit::BaseDie* _basedie {nullptr};

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