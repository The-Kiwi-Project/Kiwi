#include "./schematicinfowidget.h"
#include "qstackedwidget.h"
#include "qwidget.h"

#include <circuit/basedie.hh>

#include "widget/schematic/info/exportwidget.h"
#include "widget/schematic/info/netinfowidget.h"
#include "widget/schematic/info/tpdinfowidget.h"
#include "widget/schematic/info/viewinfowidget.h"
#include "widget/schematic/item/exportitem.h"
#include "widget/schematic/item/netitem.h"
#include "widget/schematic/item/topdieinstitem.h"
#include "widget/schematic/item/topdieinstitem.h"
#include "widget/schematic/schematiclibwidget.h"

#include <QSplitter>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QStackedWidget>

namespace kiwi::widget {

    SchematicInfoWidget::SchematicInfoWidget(circuit::BaseDie* basedie, SchematicView* view, QWidget* parent) :
        QStackedWidget{parent},
        _basedie{basedie}
    {
        this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        // View 
        this->_viewInfoWidget = new schematic::ViewInfoWidget {view, this};
        this->addWidget(this->_viewInfoWidget);

        // Net info
        this->_netInfoWidget = new schematic::NetInfoWidget {this};
        this->addWidget(this->_netInfoWidget);

        // TopDie inst
        this->_topdieInstInfoWidget = new schematic::TopDieInstInfoWidget {this};
        this->addWidget(this->_topdieInstInfoWidget);

        connect(this->_topdieInstInfoWidget, &schematic::TopDieInstInfoWidget::topDieInstRename, [this](schematic::TopDieInstanceItem* inst, const QString& name){
            this->_basedie->topdie_inst_rename(inst->topdieInst(), name.toStdString());
            inst->setName(name);
        });

        // Export inst
        this->_eportInfoWidget = new schematic::ExPortItemInfoWidget {this};
        this->addWidget(this->_eportInfoWidget);

        connect(this->_eportInfoWidget, &schematic::ExPortItemInfoWidget::exportRename, [this](schematic::ExPortItem* eport, const QString& name) {
            this->_basedie->external_port_rename(eport->exPort(), name.toStdString());
            eport->setName(name);
        });

        connect(this->_eportInfoWidget, &schematic::ExPortItemInfoWidget::exportSetCoord, [this](schematic::ExPortItem* eport, hardware::TrackCoord& coord) {
            this->_basedie->external_port_set_coord(eport->exPort(), coord);
        });

    }

    void SchematicInfoWidget::showExPortInfoWidget(schematic::ExPortItem* eport) {
        this->setCurrentWidget(this->_eportInfoWidget);
        this->_eportInfoWidget->loadExPort(eport);
    }

    void SchematicInfoWidget::showNetInfoWidget(schematic::NetItem* net) {
        this->setCurrentWidget(this->_netInfoWidget);
        this->_netInfoWidget->loadNet(net);
    }

    void SchematicInfoWidget::showTopDieInstInfoWidget(schematic::TopDieInstanceItem* inst) {
        this->setCurrentWidget(this->_topdieInstInfoWidget);
        this->_topdieInstInfoWidget->loadInst(inst);
    }

    void SchematicInfoWidget::showViewInfo() {
        this->setCurrentWidget(this->_viewInfoWidget);
    }

}