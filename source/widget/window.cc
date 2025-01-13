#include "./window.h"
#include "./view3d/view3dwidget.h"
#include "./view2d/view2dview.h"
#include "./layout/layoutview.h"
#include "./layout/layoutview.h"
#include "./schematic/schematicview.h"
#include "./schematic/schematicwidget.h"
#include "./layout/layoutwidget.h"

#include <QDebug>
#include <QResizeEvent>
#include <QVBoxLayout>
#include <QToolBar>
#include <QStackedWidget>
#include <QToolButton>

namespace kiwi::widget {

    Window::Window(hardware::Interposer* i, circuit::BaseDie* b, QWidget *parent)
        : QMainWindow{parent}
    {
        this->_toolBar = new QToolBar(this);
        this->_toolBar->setOrientation(Qt::Vertical);  // 设置为垂直方向
        this->_toolBar->setMovable(false);             // 不可拖动
        this->_toolBar->setMinimumWidth(50);
        this->_toolBar->setIconSize(QSize(40, 40));
        this->addToolBar(Qt::LeftToolBarArea, this->_toolBar);

        auto schematicButton = this->_toolBar->addAction(QIcon(":/image/image/icon/chip.png"), "");
        auto layoutButton = this->_toolBar->addAction(QIcon(":/image/image/icon/layout.png"), "");

        this->_stackedWidget = new QStackedWidget(this);

        this->_schematicWidget = new SchematicWidget{i, b, this->_schematicWidget};
        this->_stackedWidget->addWidget(this->_schematicWidget);

        this->_layoutWidget = new LayoutWidget{i, b, this->_schematicWidget};
        this->_stackedWidget->addWidget(this->_layoutWidget);

        setCentralWidget(this->_stackedWidget);

        connect(schematicButton, &QAction::triggered, [this]() {
            this->_stackedWidget->setCurrentWidget(this->_schematicWidget);
        });
        connect(layoutButton, &QAction::triggered, [this]() {
            this->_stackedWidget->setCurrentWidget(this->_layoutWidget);
        });

        this->resize(1500, 900);
    }

    Window::~Window() {}

}
