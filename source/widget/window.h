#pragma once

#include <QMainWindow>
#include "./page/welcomepage.h"
#include "./page/workpage.h"
#include "./page/aboutpage.h"
#include "./base/framelesswindow.h"
#include "./base/sidebar.h"

namespace kiwi::hardware {
    class Interposer;
}

namespace kiwi::circuit {
    class BaseDie;
}

namespace kiwi::widget {

    class View2DWidget;
    class View3DWidget;

    class Window : public FramelessWindow {
        Q_OBJECT

    public:
        Window(hardware::Interposer* i, circuit::BaseDie* b, QWidget *parent = nullptr);
        ~Window();

    private:
        void resizePages(QResizeEvent* event);

    private:
        virtual void showEvent(QShowEvent* event) override;
        virtual bool eventFilter(QObject* object, QEvent* event) override;

    private:
        QHBoxLayout* _mainLayout = nullptr;
        QWidget* _placeHolderWidget = nullptr;
        SideBar* _sideBar = nullptr;

        WelcomePage* _welcomePage = nullptr;
        WorkPage* _workPage = nullptr;
        AboutPage* _aboutPage = nullptr;
    };

    class SWindow : public QWidget 
    {
        Q_OBJECT

    public:
        SWindow(hardware::Interposer* i, circuit::BaseDie* b, QWidget *parent = nullptr);
        ~SWindow();

    private:
        View2DWidget* _renderWidget {nullptr};
    };

}

