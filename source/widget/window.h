#pragma once

#include <QMainWindow>

namespace kiwi::hardware {
    class Interposer;
}

namespace kiwi::circuit {
    class BaseDie;
}

class QToolBar;
class QStackedWidget;

namespace kiwi::widget {

    class SchematicWidget;
    class LayoutWidget;

    class Window : public QMainWindow 
    {
        Q_OBJECT

    public:
        Window(hardware::Interposer* i, circuit::BaseDie* b, QWidget *parent = nullptr);
        ~Window();

    private:
        QToolBar* _toolBar {nullptr};
        QStackedWidget* _stackedWidget {nullptr};

        SchematicWidget* _schematicWidget {nullptr};
        LayoutWidget* _layoutWidget {nullptr};
    };

}

