#pragma once

#include <QStackedWidget>

namespace kiwi::circuit {
    class BaseDie;
};

namespace kiwi::hardware {
    class TrackCoord;
}

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

    public:
        void reload();

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
        void externalPortRename(schematic::ExternalPortItem* eport, const QString& name);
        void externalPortSetCoord(schematic::ExternalPortItem* eport, const hardware::TrackCoord& coord);
        void removeExternalPort(schematic::ExternalPortItem* eport);

        void netSyncChanged(schematic::NetItem* net, int sync);
        void netColorChanged(schematic::NetItem* net, const QColor& color);
        void netWidthChanged(schematic::NetItem* net, qreal width);
        void removeNet(schematic::NetItem* net);

        void topdieInstanceRename(schematic::TopDieInstanceItem* inst, const QString& name);
        void removeTopDieInstance(schematic::TopDieInstanceItem* inst);

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