#include "./schematicwidget.h"
#include "./schematicscene.h"
#include "./schematicview.h"
#include "./schematicinfowidget.h"
#include "./schematiclibwidget.h"

#include "./item/topdieinstitem.h"
#include "./item/netitem.h"
#include "./item/exportitem.h"

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

        this->setWindowTitle("Schematic Editor");
        this->resize(1000, 800);
    }

    void SchematicWidget::initTopdieLibWidget() {
        this->_libWidget = new SchematicLibWidget {this->_basedie, this->_splitter};
        this->_libWidget->setMinimumWidth(200);
        this->_libWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

        this->_splitter->addWidget(this->_libWidget);

        QObject::connect(
            this->_libWidget, &SchematicLibWidget::initialTopDieInst, 
            this->_scene, &SchematicScene::handleInitialTopDie);

        QObject::connect(
            this->_libWidget, &SchematicLibWidget::addExport, 
            this->_scene, &SchematicScene::handleAddExport);
    }

    void SchematicWidget::initSchematicView(hardware::Interposer* interposer, circuit::BaseDie* basedie) {
        this->_view = new SchematicView {interposer, basedie};
        this->_view->setScene(this->_scene);
        this->_view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        this->_view->adjustSceneRect();

        this->_splitter->addWidget(this->_view);
    }

    void SchematicWidget::initInfoWidget() {
        this->_infoWidget = new SchematicInfoWidget{this->_basedie, this->_scene, this->_view, this->_splitter};
        this->_infoWidget->setMinimumWidth(250);
        this->_infoWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

        this->_splitter->addWidget(this->_infoWidget);

        QObject::connect(
            this->_scene, &SchematicScene::viewSelected, 
            this->_infoWidget, &SchematicInfoWidget::showViewInfo);

        QObject::connect(
            this->_scene, &SchematicScene::netSelected, 
            this->_infoWidget, &SchematicInfoWidget::showNetInfoWidget);

        QObject::connect(
            this->_scene, &SchematicScene::exportSelected, 
            this->_infoWidget, &SchematicInfoWidget::showExPortInfoWidget);

        QObject::connect(
            this->_scene, &SchematicScene::topdieInstSelected, 
            this->_infoWidget, &SchematicInfoWidget::showTopDieInstanceInfoWidget);
    }

}