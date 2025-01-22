#pragma once

#include "qwidget.h"
#include <QWidget>

namespace kiwi::hardware {
    class Interposer;
};

namespace kiwi::circuit {
    class BaseDie;
};

namespace kiwi::widget {

    class View2DView;
    class View2DScene;

    class View2DWidget : public QWidget {
    public:
        View2DWidget(
            hardware::Interposer* interposer, 
            circuit::BaseDie* basedie,
            QWidget* parent = nullptr);

    public:
        void reload();

    protected:
        View2DScene* _scene {nullptr};
        View2DView* _view {nullptr};

        hardware::Interposer* _interposer {nullptr};
        circuit::BaseDie* _basedie {nullptr};
    };

}