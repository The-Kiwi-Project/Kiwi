#include "./schematicinfowidget.h"
#include "qstackedwidget.h"
#include "qwidget.h"

#include <circuit/basedie.hh>

#include "./info/exportwidget.h"
#include "./info/netinfowidget.h"
#include "./info/tpdinfowidget.h"
#include "./info/viewinfowidget.h"
#include "./item/exportitem.h"
#include "./item/netitem.h"
#include "./item/topdieinstitem.h"
#include "./item/topdieinstitem.h"
#include "./schematicscene.h"

#include <debug/debug.hh>

#include <QSplitter>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QStackedWidget>

namespace kiwi::widget {

    SchematicInfoWidget::SchematicInfoWidget(
        circuit::BaseDie* basedie, 
        SchematicScene* scene,
        SchematicView* view, 
        QWidget* parent
    ) :
        QStackedWidget{parent},
        _basedie{basedie},
        _scene{scene}
    {
        this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        // View 
        this->_viewInfoWidget = new schematic::ViewInfoWidget {view, this};
        this->addWidget(this->_viewInfoWidget);

        // Net info
        this->_netInfoWidget = new schematic::NetInfoWidget {this};
        this->addWidget(this->_netInfoWidget);

        connect(this->_netInfoWidget, &schematic::NetInfoWidget::netSyncChanged, [this] (schematic::NetItem* net, int sync) {
            this->_basedie->connection_set_sync(net->unwrap(), sync);
        });

        connect(this->_netInfoWidget, &schematic::NetInfoWidget::removeNet, [this] (schematic::NetItem* net) {
            this->_basedie->remove_connection(net->unwrap());
            this->_scene->removeNet(net);
        });

        // TopDie inst
        this->_topdieInstInfoWidget = new schematic::TopDieInstanceInfoWidget {this};
        this->addWidget(this->_topdieInstInfoWidget);

        connect(this->_topdieInstInfoWidget, &schematic::TopDieInstanceInfoWidget::topdieInstanceRename, [this](schematic::TopDieInstanceItem* inst, const QString& name){
            this->_basedie->topdie_inst_rename(inst->unwrap(), name.toStdString());
            inst->setName(name);
        });

        connect(this->_topdieInstInfoWidget, &schematic::TopDieInstanceInfoWidget::removeTopDieInstance, [this] (schematic::TopDieInstanceItem* inst) {
            this->_basedie->remove_topdie_inst(inst->unwrap());
            this->_scene->removeTopDieInstance(inst);
        });

        // Export inst
        this->_eportInfoWidget = new schematic::ExternalPortInfoWidget {this};
        this->addWidget(this->_eportInfoWidget);

        connect(this->_eportInfoWidget, &schematic::ExternalPortInfoWidget::externalPortRename, [this](schematic::ExternalPortItem* eport, const QString& name) {
            this->_basedie->external_port_rename(eport->unwrap(), name.toStdString());
            eport->setName(name);
        });

        connect(this->_eportInfoWidget, &schematic::ExternalPortInfoWidget::externalPortSetCoord, [this](schematic::ExternalPortItem* eport, hardware::TrackCoord& coord) {
            this->_basedie->external_port_set_coord(eport->unwrap(), coord);
        });

        connect(this->_eportInfoWidget, &schematic::ExternalPortInfoWidget::removeExternalPort, [this] (schematic::ExternalPortItem* eport) {
            this->_basedie->remove_external_port(eport->unwrap());
            this->_scene->removeExternalPort(eport);
        });
    }

    void SchematicInfoWidget::showExPortInfoWidget(schematic::ExternalPortItem* eport) {
        this->setCurrentWidget(this->_eportInfoWidget);
        this->_eportInfoWidget->loadExternalPort(eport);
    }

    void SchematicInfoWidget::showNetInfoWidget(schematic::NetItem* net) {
        this->setCurrentWidget(this->_netInfoWidget);
        this->_netInfoWidget->loadNet(net);
    }

    void SchematicInfoWidget::showTopDieInstanceInfoWidget(schematic::TopDieInstanceItem* inst) {
        this->setCurrentWidget(this->_topdieInstInfoWidget);
        this->_topdieInstInfoWidget->loadTopDieInstance(inst);
    }

    void SchematicInfoWidget::showViewInfo() {
        this->setCurrentWidget(this->_viewInfoWidget);
    }

}