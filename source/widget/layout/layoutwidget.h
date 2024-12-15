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
   
    class LayoutWidget : public QGraphicsView {
        
        enum {
            COB_INTERVAL = 50,
        };

    public:
        explicit LayoutWidget(
            hardware::Interposer* interposer, 
            circuit::BaseDie* basedie,
            QWidget *parent = nullptr);

        ~LayoutWidget() noexcept;

    protected:
        virtual void wheelEvent(QWheelEvent *event) override;

    protected:
        hardware::Interposer* _interposer {nullptr};
        circuit::BaseDie* _basedie {nullptr};

        QGraphicsScene* _scene {nullptr};
    };

}