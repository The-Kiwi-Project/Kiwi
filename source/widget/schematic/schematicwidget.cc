#include "./schematicwidget.h"
#include "./schematicview.h"
#include "qnamespace.h"
#include "qsplitter.h"
#include "qwidget.h"
#include <QSplitter>
#include <QVBoxLayout>

namespace kiwi::widget {

    SchematicWidget::SchematicWidget(hardware::Interposer* interposer, circuit::BaseDie* basedie, QWidget *parent) :
        QWidget{parent},
        _interposer{interposer},
        _basedie{basedie}
    {
        auto splitter = new QSplitter{Qt::Horizontal, this};

        this->_topdieLibWidget = new QWidget{};
        this->_topdieLibWidget->setMinimumWidth(200);
        this->_topdieLibWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

        this->_view = new SchematicView {interposer, basedie};
        this->_view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        this->_messageWidget = new QWidget{};
        this->_messageWidget->setMinimumWidth(200);
        this->_messageWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);


        splitter->addWidget(this->_topdieLibWidget);
        splitter->addWidget(this->_view);
        splitter->addWidget(this->_messageWidget);

        // splitter->setStretchFactor(0, 1); // 左侧占比
        // splitter->setStretchFactor(1, 2); // 中间占比
        // splitter->setStretchFactor(2, 1); // 右侧占比
        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->addWidget(splitter); // 将 QSplitter 添加到布局
        layout->setContentsMargins(0, 0, 0, 0); // 去掉内边距
    
        this->setWindowTitle("Schematic Editor");
        this->resize(1000, 800);
    }

}