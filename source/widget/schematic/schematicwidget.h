#pragma once

#include "qwidget.h"
#include <QWidget>
#include <QStackedWidget>
#include <QSplitter>

namespace kiwi::hardware {
    class Interposer;
};

namespace kiwi::circuit {
    class BaseDie;
};

namespace kiwi::widget {

    class SchematicView;
    class SchematicScene;

    class SchematicWidget : public QWidget {
    public:
        explicit SchematicWidget(
            hardware::Interposer* interposer, 
            circuit::BaseDie* basedie,
            QWidget *parent = nullptr);

    private:
        void initTopdieLibWidget();
        void initSchematicView(hardware::Interposer* interposer, circuit::BaseDie* basedie);
        void initInfoWidget();

    protected:
        QSplitter* _splitter {nullptr};

        SchematicView*  _view {nullptr};  
        SchematicScene* _scene {nullptr};

        hardware::Interposer* _interposer {nullptr};
        circuit::BaseDie*     _basedie {nullptr};
    };

}