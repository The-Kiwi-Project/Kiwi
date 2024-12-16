#pragma once

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
   
    class View2DView : public QGraphicsView {
        
        enum {
            COB_INTERVAL = 50,
        };

    public:
        explicit View2DView(
            hardware::Interposer* interposer, 
            circuit::BaseDie* basedie,
            QWidget *parent = nullptr);

        ~View2DView() noexcept;

    protected:
        virtual void wheelEvent(QWheelEvent *event) override;

    protected:
        hardware::Interposer* _interposer {nullptr};
        circuit::BaseDie* _basedie {nullptr};

        QGraphicsScene* _scene {nullptr};
    };

}