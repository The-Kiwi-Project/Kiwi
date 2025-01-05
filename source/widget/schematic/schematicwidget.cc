#include "./schematicwidget.h"
#include "./schematicscene.h"
#include "./schematicview.h"
#include "circuit/topdie/topdie.hh"
#include "qboxlayout.h"
#include "qglobal.h"
#include "qlayoutitem.h"
#include "qnamespace.h"
#include "qobject.h"
#include "qsplitter.h"
#include "qstackedwidget.h"
#include "qwidget.h"
#include "widget/schematic/info/exportwidget.h"
#include "widget/schematic/info/netinfowidget.h"
#include "widget/schematic/info/tpdinfowidget.h"
#include "widget/schematic/info/viewinfowidget.h"
#include "widget/schematic/item/exportitem.h"
#include "widget/schematic/item/netitem.h"
#include "widget/schematic/item/topdieinstitem.h"
#include "widget/schematic/schematiclibwidget.h"
#include <QSplitter>
#include <QVBoxLayout>
#include <QLineEdit>

namespace kiwi::widget {

    SchematicWidget::SchematicWidget(hardware::Interposer* interposer, circuit::BaseDie* basedie, QWidget *parent) :
        QWidget{parent},
        _interposer{interposer},
        _basedie{basedie}
    {
        this->_scene = new SchematicScene{this->_basedie};

        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->setContentsMargins(10, 10, 10, 10);

        this->_splitter = new QSplitter{Qt::Horizontal, this};
        layout->addWidget(this->_splitter);

        this->initTopdieLibWidget();
        this->initSchematicView(interposer, basedie);
        this->initInfoWidget();
    }

    void SchematicWidget::initTopdieLibWidget() {
        auto topdieLibWidget = new SchematicLibWidget {this->_basedie, this->_splitter};
        topdieLibWidget->setMinimumWidth(200);
        topdieLibWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

        this->_splitter->addWidget(topdieLibWidget);

        QObject::connect(topdieLibWidget, &SchematicLibWidget::initialTopDieInst, [this] (circuit::TopDie* topdie) {
            this->_scene->handleInitialTopDie(topdie);
        });
    }

    void SchematicWidget::initSchematicView(hardware::Interposer* interposer, circuit::BaseDie* basedie) {
        this->_view = new SchematicView {interposer, basedie, this->_scene};
        this->_view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        this->_splitter->addWidget(this->_view);
    }

    void SchematicWidget::initInfoWidget() {
        auto infoWidget = new QWidget {this->_splitter};
        infoWidget->setMinimumWidth(250);

        auto infoLayout = new QVBoxLayout {};
        infoWidget->setLayout(infoLayout);
    
        auto stackedWidget = new QStackedWidget {infoWidget};
        stackedWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        // View 
        auto viewinfoWidget = new schematic::ViewInfoWidget {this->_view, stackedWidget};
        stackedWidget->addWidget(viewinfoWidget);

        // Net info
        auto netinfoWidget = new schematic::NetInfoWidget {stackedWidget};
        stackedWidget->addWidget(netinfoWidget);

        // TopDie inst
        auto tpdinfoWidget = new schematic::TopdieInstInfoWidget {stackedWidget};
        stackedWidget->addWidget(tpdinfoWidget);

        // Export inst
        auto exportWidget = new schematic::ExPortItemInfoWidget {stackedWidget};
        stackedWidget->addWidget(exportWidget);

        infoLayout->addWidget(stackedWidget);
        this->_splitter->addWidget(infoWidget);
    
        this->setWindowTitle("Schematic Editor");
        this->resize(1000, 800);

        QObject::connect(this->_scene, &SchematicScene::viewSelected, [viewinfoWidget, stackedWidget] () {
            stackedWidget->setCurrentWidget(viewinfoWidget);
        });

        QObject::connect(this->_scene, &SchematicScene::netSelected, [netinfoWidget, stackedWidget] (schematic::NetItem* net) {
            netinfoWidget->loadNet(net);
            stackedWidget->setCurrentWidget(netinfoWidget);
        });

        QObject::connect(this->_scene, &SchematicScene::exportSelected, [exportWidget, stackedWidget] (schematic::ExPortItem* eport) {
            exportWidget->loadExPort(eport);
            stackedWidget->setCurrentWidget(exportWidget);
        });

        QObject::connect(this->_scene, &SchematicScene::topdieInstSelected, [tpdinfoWidget, stackedWidget] (schematic::TopDieInstanceItem* inst) {
            tpdinfoWidget->loadInst(inst);
            stackedWidget->setCurrentWidget(tpdinfoWidget);
        });
    }

}