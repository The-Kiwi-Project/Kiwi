#pragma once

#include "qobjectdefs.h"
#include "qwidget.h"
#include "widget/schematic/schematicinfowidget.h"
#include "widget/schematic/schematiclibwidget.h"
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
        Q_OBJECT
        
    public:
        explicit SchematicWidget(
            hardware::Interposer* interposer, 
            circuit::BaseDie* basedie,
            QWidget *parent = nullptr);

    public:
        void reload();

    signals:
        // MARK: More detail infomation
        void layoutChanged();

    private:
        void initTopdieLibWidget();
        void initSchematicView(hardware::Interposer* interposer, circuit::BaseDie* basedie);
        void initInfoWidget();

    protected:
        QSplitter* _splitter {nullptr};

        SchematicScene* _scene {nullptr};
        SchematicLibWidget* _libWidget {nullptr};
        SchematicView*  _view {nullptr};  
        SchematicInfoWidget* _infoWidget {nullptr};

        hardware::Interposer* _interposer {nullptr};
        circuit::BaseDie*     _basedie {nullptr};
    };

}