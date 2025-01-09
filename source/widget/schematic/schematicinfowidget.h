#pragma once

#include "qobjectdefs.h"
#include <QStackedWidget>

namespace kiwi::circuit {
    class BaseDie;
};

namespace kiwi::widget {

    namespace schematic {
        class ExternalPortInfoWidget;
        class NetInfoWidget;
        class TopDieInstanceInfoWidget;
        class ViewInfoWidget;

        class ExternalPortItem;
        class NetItem;
        class TopDieInstanceItem;
    }

    class SchematicView;
    class SchematicScene;

    class SchematicInfoWidget : public QStackedWidget {
        Q_OBJECT

    public:
        SchematicInfoWidget(
            circuit::BaseDie* basedie, 
            SchematicScene* scene,
            SchematicView* view, 
            QWidget* parent = nullptr);

    private:
        void createViewInfoWidget();
        void createExternalPortInfoWidget();
        void createNetInfoWidget();
        void createTopDieInstanceInfoWidget();
        
    public:
        void showViewInfo();
        void showExPortInfoWidget(schematic::ExternalPortItem*);
        void showNetInfoWidget(schematic::NetItem*);
        void showTopDieInstanceInfoWidget(schematic::TopDieInstanceItem*);

    private:
        circuit::BaseDie* _basedie {nullptr};
        SchematicScene* _scene {nullptr};
        SchematicView* _view {nullptr};

        schematic::ViewInfoWidget* _viewInfoWidget {nullptr};
        schematic::ExternalPortInfoWidget* _eportInfoWidget {nullptr};
        schematic::NetInfoWidget* _netInfoWidget {nullptr};
        schematic::TopDieInstanceInfoWidget* _topdieInstInfoWidget {nullptr};
    };

}