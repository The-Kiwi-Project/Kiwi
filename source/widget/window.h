#pragma once

#include "std/file.hh"
#include "std/memory.hh"
#include "std/utility.hh"
#include <QMainWindow>

namespace kiwi::hardware {
    class Interposer;
}

namespace kiwi::circuit {
    class BaseDie;
}

class QToolBar;
class QStackedWidget;
class QMenuBar;

namespace kiwi::widget {

    class SchematicWidget;
    class LayoutWidget;

    class Window : public QMainWindow {
        Q_OBJECT

    public:
        Window(QWidget *parent = nullptr);
        ~Window();

    private:
        void createSystem();
        void createMenuBar();
        void createToolBar();
        void createCentralWidget();

    private:
        void loadConfig();
        void saveConfig();
        void saveConfigAs();

    private:
        auto hasConfigPath() -> bool;

    private:
        QMenuBar* _menuBar {nullptr};
        QToolBar* _toolBar {nullptr};
        QStackedWidget* _stackedWidget {nullptr};

        SchematicWidget* _schematicWidget {nullptr};
        LayoutWidget* _layoutWidget {nullptr};

    private:
        std::Box<hardware::Interposer> _interposer {nullptr};
        std::Box<circuit::BaseDie> _basedie {nullptr};

        std::Option<std::FilePath> _configPath {}; 
    };

}

