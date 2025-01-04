#include "./schematicwidget.h"
#include "./schematicscene.h"
#include "./schematicview.h"
#include "qboxlayout.h"
#include "qnamespace.h"
#include "qsplitter.h"
#include "qwidget.h"
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

        auto splitter = new QSplitter{Qt::Horizontal, this};
        layout->addWidget(splitter);

        this->_topdieLibWidget = new QWidget{};
        this->_topdieLibWidget->setMinimumWidth(200);
        this->_topdieLibWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

        this->_view = new SchematicView {interposer, basedie, this->_scene};
        this->_view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        this->_infoContainerWidget = new QWidget{};
        this->_infoContainerWidget->setMinimumWidth(200);
        this->_infoContainerWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        this->_infoContainerWidget->setStyleSheet("background-color: lightgray;");
        auto infoLayout = new QVBoxLayout {this->_infoContainerWidget};
        infoLayout->addStretch();

        splitter->addWidget(this->_topdieLibWidget);
        splitter->addWidget(this->_view);
        splitter->addWidget(this->_infoContainerWidget);
    
        this->setWindowTitle("Schematic Editor");
        this->resize(1000, 800);

        QObject::connect(this->_scene, &SchematicScene::infoWidgetChanged, [this, infoLayout] (QWidget* widget) {
            if (this->_infoWidget) {
                infoLayout->removeWidget(this->_infoWidget);
                delete this->_infoWidget;
            }
            infoLayout->insertWidget(0, widget);
            this->_infoWidget = widget;
        });
    }

}