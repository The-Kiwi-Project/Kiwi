#pragma once

#include <QWidget>
#include <QSplitter>

namespace kiwi::hardware {
    class Interposer;
};

namespace kiwi::circuit {
    class BaseDie;
};

namespace kiwi::widget {

    class LayoutScene;
    class LayoutView;
    class LayoutInfoWidget;

    class LayoutWidget : public QWidget {
        Q_OBJECT

    public:
        explicit LayoutWidget(
            hardware::Interposer* interposer, 
            circuit::BaseDie* basedie,
            QWidget *parent = nullptr
        );

    public:
        void reload();

    private:
        QSplitter* _splitter {nullptr};

        LayoutScene* _scene {nullptr};
        LayoutView*  _view {nullptr};  
        LayoutInfoWidget* _infoWidget {nullptr};

        hardware::Interposer* _interposer {nullptr};
        circuit::BaseDie*     _basedie {nullptr};
    };

}