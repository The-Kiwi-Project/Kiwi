#include "./schematicwidget.h"
#include "./schematicscene.h"
#include "./schematicview.h"
#include "qboxlayout.h"
#include "qlayoutitem.h"
#include "qnamespace.h"
#include "qsplitter.h"
#include "qstackedwidget.h"
#include "qwidget.h"
#include "widget/schematic/info/netinfowidget.h"
#include "widget/schematic/item/netitem.h"
#include <QSplitter>
#include <QVBoxLayout>
#include <QLineEdit>

namespace kiwi::widget {

    SchematicWidget::SchematicWidget(hardware::Interposer* interposer, circuit::BaseDie* basedie, QWidget *parent) :
        QWidget{parent},
        _interposer{interposer},
        _basedie{basedie}
    {
        this->_scene = new SchematicScene{};

        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->setContentsMargins(10, 10, 10, 10);

        this->_splitter = new QSplitter{Qt::Horizontal, this};
        layout->addWidget(this->_splitter);

        this->initTopdieLibWidget();
        this->initSchematicView(interposer, basedie);
        this->initInfoWidget();
    }

    void SchematicWidget::initTopdieLibWidget() {
        auto topdieLibWidget = new QWidget{this->_splitter};
        topdieLibWidget->setMinimumWidth(200);
        topdieLibWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

        this->_splitter->addWidget(topdieLibWidget);
    }

    void SchematicWidget::initSchematicView(hardware::Interposer* interposer, circuit::BaseDie* basedie) {
        this->_view = new SchematicView {interposer, basedie, this->_scene};
        this->_view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        this->_splitter->addWidget(this->_view);
    }

    void SchematicWidget::initInfoWidget() {
        auto infoWidget = new QWidget {this->_splitter};
        infoWidget->setStyleSheet("background-color: lightgray;");
        infoWidget->setMinimumWidth(200);

        auto infoLayout = new QVBoxLayout {};
        infoWidget->setLayout(infoLayout);
    
        auto infoContainer = new QStackedWidget {infoWidget};
        infoContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

        auto netinfoWidget = new schematic::NetInfoWidget {infoContainer};
        infoContainer->addWidget(netinfoWidget);

        infoLayout->addWidget(infoContainer, 0);
        infoLayout->addStretch(1);

        this->_splitter->addWidget(infoWidget);
    
        this->setWindowTitle("Schematic Editor");
        this->resize(1000, 800);

        QObject::connect(this->_scene, &SchematicScene::netSelected, [netinfoWidget, infoContainer] (schematic::NetItem* net) {
            netinfoWidget->loadNet(net);
            infoContainer->setCurrentWidget(netinfoWidget);
        });
    }

}