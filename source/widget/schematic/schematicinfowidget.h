#pragma once

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

    class SchematicInfoWidget : public QStackedWidget {
        Q_OBJECT

    public:
        SchematicInfoWidget(circuit::BaseDie* basedie, SchematicView* view, QWidget* parent = nullptr);
        
    public:
        void showExPortInfoWidget(schematic::ExternalPortItem*);
        void showNetInfoWidget(schematic::NetItem*);
        void showTopDieInstanceInfoWidget(schematic::TopDieInstanceItem*);
        void showViewInfo();

    private:
        circuit::BaseDie* _basedie {nullptr};

        schematic::ExternalPortInfoWidget* _eportInfoWidget {nullptr};
        schematic::NetInfoWidget* _netInfoWidget {nullptr};
        schematic::TopDieInstanceInfoWidget* _topdieInstInfoWidget {nullptr};
        schematic::ViewInfoWidget* _viewInfoWidget {nullptr};
    };

}