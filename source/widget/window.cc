#include "./window.h"
#include "./prthread.h"
#include "./view2d/view2dview.h"
#include "./view3d/view3dwidget.h"
#include "./schematic/schematicwidget.h"
#include "./layout/layoutwidget.h"

#include "algo/netbuilder/netbuilder.hh"
#include "algo/router/maze/mazererouter.hh"
#include "qaction.h"
#include "qmessagebox.h"
#include "widget/setting/settingwidget.h"
#include <algo/router/route.hh>

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
#include <QMessageBox>
#include <QLabel>
#include <QThread>
#include <QStatusBar>

namespace kiwi::widget {

    Window::Window(QWidget *parent)
        : QMainWindow{parent}
    {
        this->createSystem();
        
        this->createMenuBar();
        this->createToolBar();
        this->createCentralWidget();
        this->createStatusBar();

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
        connect(exitAction, &QAction::triggered, this, &Window::close);

        // ====================== View ======================
        auto viewMenu= new QMenu("View", this->_menuBar);
        auto themesAction = new QAction{"Themes", viewMenu};
        viewMenu->addAction(themesAction);

        this->_menuBar->addMenu(viewMenu);

        // ====================== Help ======================
        auto helpMenu= new QMenu("Help", this->_menuBar);
        auto aboutAction= new QAction("About", helpMenu);
        helpMenu->addAction(aboutAction);

        auto aboutQTAction= new QAction("About Qt", helpMenu);
        helpMenu->addAction(aboutQTAction);

        this->_menuBar->addMenu(helpMenu);
    }

    void Window::createToolBar() {
        this->_toolBar = new QToolBar(this);
        this->_toolBar->setOrientation(Qt::Vertical);
        this->_toolBar->setMovable(false);
        this->_toolBar->setMinimumWidth(50);
        this->_toolBar->setIconSize(QSize(35, 35));
        this->addToolBar(Qt::LeftToolBarArea, this->_toolBar);

        // Page button!
        auto schematicButton = this->_toolBar->addAction(QIcon(":/image/image/icon/chip.png"), "");
        auto layoutButton = this->_toolBar->addAction(QIcon(":/image/image/icon/layout.png"), "");
        auto view2DButton = this->_toolBar->addAction(QIcon(":/image/image/icon/view2d.png"), "");
        auto view3DButton = this->_toolBar->addAction(QIcon(":/image/image/icon/view3d.png"), "");

        connect(schematicButton, &QAction::triggered, [this]() {
            this->_stackedWidget->setCurrentWidget(this->_schematicWidget);
        });
        connect(layoutButton, &QAction::triggered, [this]() {
            this->_stackedWidget->setCurrentWidget(this->_layoutWidget);
        });
        connect(view2DButton, &QAction::triggered, [this] () {
            this->_stackedWidget->setCurrentWidget(this->_view2DWidget);
        });
        connect(view3DButton, &QAction::triggered, [this] () {
            this->_stackedWidget->setCurrentWidget(this->_view3DWidget);
        });

        this->_toolBar->addSeparator();

        QWidget *stretch = new QWidget(this->_toolBar);
        stretch->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        this->_toolBar->addWidget(stretch);

        auto prButton = this->_toolBar->addAction(QIcon{":/image/image/icon/execute.png"}, "");
        connect(prButton, &QAction::triggered, this, &Window::executePR);

        this->_toolBar->addSeparator();

        // Run button!
        auto settingButton = this->_toolBar->addAction(QIcon{":/image/image/icon/setting.png"}, "");
        connect(settingButton, &QAction::triggered, [this] () {
            this->_stackedWidget->setCurrentWidget(this->_settingWidget);
        });
    }

    void Window::createCentralWidget() {
        this->_stackedWidget = new QStackedWidget(this);

        this->_schematicWidget = new SchematicWidget{this->_interposer.get(), this->_basedie.get(), this->_schematicWidget};
        this->_stackedWidget->addWidget(this->_schematicWidget);

        this->_layoutWidget = new LayoutWidget{this->_interposer.get(), this->_basedie.get(), this->_schematicWidget};
        this->_stackedWidget->addWidget(this->_layoutWidget);

        this->_view2DWidget = new View2DView {this->_interposer.get(), this->_basedie.get(), this};
        this->_stackedWidget->addWidget(this->_view2DWidget);

        this->_view3DWidget = new View3DWidget {this->_interposer.get(), this->_basedie.get(), this};
        this->_stackedWidget->addWidget(this->_view3DWidget);

        this->_settingWidget = new SettingWidget {this};
        this->_stackedWidget->addWidget(this->_settingWidget); 

        this->setCentralWidget(this->_stackedWidget);

        connect(this->_schematicWidget, &SchematicWidget::layoutChanged, this->_layoutWidget, &LayoutWidget::reload);
    }

    void Window::createStatusBar() {
        auto statusBar = this->statusBar();

        auto status1 = new QLabel{"Kiwi", this};
        status1->setAlignment(Qt::AlignCenter);                    

        status1->setMinimumWidth(200); 

        statusBar->addPermanentWidget(status1);
    }

    void Window::loadConfig() try {
        if (this->_finishPR) {
            QMessageBox::critical(\
                this,
                "Load Config",
                "Can't load new config after finishing P&R"
            );
            return;
        }

        if (this->hasConfigPath()) {
            auto reply = QMessageBox::question(nullptr, 
                        "Load Config", 
                        "A configuration already exists.\nDo you want to delete the original configuration and import it?", 
                        QMessageBox::Yes | QMessageBox::No);
            if (reply == QMessageBox::No) {
                return;
            }
        }

        // Load a new one!
        auto filePath = QFileDialog::getExistingDirectory(this, "Select Config path");
        if (!filePath.isEmpty()) {
            // MARK: For loss origin data, it should be these, buf box<net> in basedie...
            // Once faild, loss all 

            // auto configPath = std::FilePath{filePath.toStdString()};
            // auto [i, b] = parse::read_config(configPath);

            // this->_interposer->clear();
            // this->_basedie->clear();

            // *(this->_interposer) = std::move(*i);
            // *(this->_basedie) = std::move(*b);

            // this->_schematicWidget->reload();
            // this->_layoutWidget->reload();

            // this->_configPath.emplace(std::move(configPath));

            auto configPath = std::FilePath{filePath.toStdString()};

            this->_interposer->clear();
            this->_basedie->clear();

            parse::read_config(configPath, this->_interposer.get(), this->_basedie.get());

            this->_schematicWidget->reload();
            this->_layoutWidget->reload();

            this->_configPath.emplace(std::move(configPath));
        }
    }
    QMESSAGEBOX_REPORT_EXCEPTION("Load Config")

    void Window::saveConfig() {
        
    }

    void Window::saveConfigAs() {

    }

    void Window::executePR() try {
        if (this->_finishPR) {
            QMessageBox::critical(
                this,
                "Execute P&R",
                "P&R already finished!"
            );

            return;
        }

        // MARK, if faild...

        auto dialog = QDialog(this);
        dialog.setWindowTitle("working...");
        dialog.setModal(true);

        QVBoxLayout layout(&dialog);
        auto label = QLabel("Executing P&R");
        auto font = label.font();
        font.setPointSize(20);
        font.setBold(true);
        label.setFont(font);
        label.setAlignment(Qt::AlignCenter);
        layout.addWidget(&label);
        dialog.setFixedSize(400, 200);

        auto *worker = new PRThread{this->_interposer.get(), this->_basedie.get()};
        connect(worker, &PRThread::prFinished, &dialog, &QDialog::accept);
        connect(worker, &PRThread::finished, worker, &QObject::deleteLater);
        worker->start();

        dialog.exec();

        ////////////////////////////////////////////////

        this->_view2DWidget->displayRoutingResult();
        this->_view3DWidget->displayRoutingResult();

        this->disableEdit();
        this->_finishPR = true;
    }
    QMESSAGEBOX_REPORT_EXCEPTION("Execute Place & Routing")

    auto Window::hasConfigPath() -> bool {
        return this->_configPath.has_value();
    }

    void Window::disableEdit() { 
        this->_schematicWidget->setEnabled(false);
        this->_layoutWidget->setEnabled(false);
    }

    Window::~Window() {}

}
