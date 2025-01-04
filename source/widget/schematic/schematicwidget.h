#pragma once

#include <QWidget>

namespace kiwi::hardware {
    class Interposer;
};

namespace kiwi::circuit {
    class BaseDie;
};

namespace kiwi::widget {

    class SchematicView;

    class SchematicWidget : public QWidget {
    public:
        explicit SchematicWidget(
            hardware::Interposer* interposer, 
            circuit::BaseDie* basedie,
            QWidget *parent = nullptr);

    protected:
        QWidget*       _topdieLibWidget {nullptr};
        SchematicView* _view {nullptr};
        QWidget*       _messageWidget {nullptr};

        hardware::Interposer* _interposer {nullptr};
        circuit::BaseDie*     _basedie {nullptr};
    };

}