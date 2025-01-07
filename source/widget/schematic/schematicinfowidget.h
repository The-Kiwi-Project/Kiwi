#pragma once

#include <QStackedWidget>

namespace kiwi::circuit {
    class BaseDie;
};

namespace kiwi::widget {

    namespace schematic {
        class ExPortItemInfoWidget;
        class NetInfoWidget;
        class TopDieInstInfoWidget;
        class ViewInfoWidget;

        class ExPortItem;
        class NetItem;
        class TopDieInstanceItem;
    }

    class SchematicView;

    class SchematicInfoWidget : public QStackedWidget {
        Q_OBJECT

    public:
        SchematicInfoWidget(circuit::BaseDie* basedie, SchematicView* view, QWidget* parent = nullptr);
        
    public:
        void showExPortInfoWidget(schematic::ExPortItem*);
        void showNetInfoWidget(schematic::NetItem*);
        void showTopDieInstInfoWidget(schematic::TopDieInstanceItem*);
        void showViewInfo();

    private:
        circuit::BaseDie* _basedie {nullptr};

        schematic::ExPortItemInfoWidget* _eportInfoWidget {nullptr};
        schematic::NetInfoWidget* _netInfoWidget {nullptr};
        schematic::TopDieInstInfoWidget* _topdieInstInfoWidget {nullptr};
        schematic::ViewInfoWidget* _viewInfoWidget {nullptr};
    };

}