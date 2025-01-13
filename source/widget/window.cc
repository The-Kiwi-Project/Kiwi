#include "./window.h"
#include "./view3d/view3dwidget.h"
#include "./view2d/view2dview.h"
#include "./layout/layoutview.h"
#include "./layout/layoutview.h"
#include "./schematic/schematicview.h"
#include "./schematic/schematicwidget.h"
#include "./layout/layoutwidget.h"

#include <parse/reader/module.hh>
#include <widget/frame/msgexception.h>

#include <hardware/interposer.hh>
#include <circuit/basedie.hh>

#include <std/exception.hh>

#include <QDebug>
#include <QResizeEvent>
#include <QVBoxLayout>
#include <QToolBar>
#include <QStackedWidget>
#include <QToolButton>
#include <QMenuBar>
#include <QFileDialog>

namespace kiwi::widget {

    Window::Window(QWidget *parent)
        : QMainWindow{parent}
    {
        this->createSystem();
        
        this->createMenuBar();
        this->createToolBar();
        this->createCentralWidget();

        this->resize(1500, 900);
    }

    void Window::createSystem() {
        this->_interposer = std::make_unique<hardware::Interposer>();
        this->_basedie = std::make_unique<circuit::BaseDie>();
    }

    void Window::createMenuBar() {
        this->_menuBar = this->menuBar();

        // ====================== File ======================
        auto fileMenu= new QMenu("File", this->_menuBar);

        // Load
        auto loadAction= new QAction("Load", fileMenu);
        fileMenu->addAction(loadAction);
        
        fileMenu->addSeparator(); 

        // Save
        auto saveAction= new QAction("Save", fileMenu);
        fileMenu->addAction(saveAction);

        // Save as
        auto saveAsAction= new QAction("Save as", fileMenu);
        fileMenu->addAction(saveAsAction);

        fileMenu->addSeparator(); 

        // Exit
        auto exitAction = new QAction("Exit", fileMenu);
        fileMenu->addAction(exitAction);

        this->_menuBar->addMenu(fileMenu);

        connect(loadAction, &QAction::triggered, this, &Window::loadConfig);
        connect(saveAction, &QAction::triggered, this, &Window::saveConfig);
        connect(saveAsAction, &QAction::triggered, this, &Window::saveConfigAs);
    }

    void Window::createToolBar() {
        this->_toolBar = new QToolBar(this);
        this->_toolBar->setOrientation(Qt::Vertical);
        this->_toolBar->setMovable(false);
        this->_toolBar->setMinimumWidth(50);
        this->_toolBar->setIconSize(QSize(40, 40));
        this->addToolBar(Qt::LeftToolBarArea, this->_toolBar);

        auto schematicButton = this->_toolBar->addAction(QIcon(":/image/image/icon/chip.png"), "");
        auto layoutButton = this->_toolBar->addAction(QIcon(":/image/image/icon/layout.png"), "");

        connect(schematicButton, &QAction::triggered, [this]() {
            this->_stackedWidget->setCurrentWidget(this->_schematicWidget);
        });
        connect(layoutButton, &QAction::triggered, [this]() {
            this->_stackedWidget->setCurrentWidget(this->_layoutWidget);
        });;
    }

    void Window::createCentralWidget() {
        this->_stackedWidget = new QStackedWidget(this);

        this->_schematicWidget = new SchematicWidget{this->_interposer.get(), this->_basedie.get(), this->_schematicWidget};
        this->_stackedWidget->addWidget(this->_schematicWidget);

        this->_layoutWidget = new LayoutWidget{this->_interposer.get(), this->_basedie.get(), this->_schematicWidget};
        this->_stackedWidget->addWidget(this->_layoutWidget);

        this->setCentralWidget(this->_stackedWidget);
    }

    void Window::loadConfig() try {
        auto filePath = QFileDialog::getExistingDirectory(this, "Select Config path");
        if (!filePath.isEmpty()) {
            auto configPath = std::FilePath{filePath.toStdString()};
            parse::read_config(configPath, this->_interposer.get(), this->_basedie.get());
        }
    }
    QMESSAGEBOX_REPORT_EXCEPTION("Load Config")

    void Window::saveConfig() {
        
    }

    void Window::saveConfigAs() {

    }

    Window::~Window() {}

}
