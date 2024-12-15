#pragma once

#include <QMainWindow>
#include "./frame/page/welcomepage.h"
#include "./frame/page/workpage.h"
#include "./frame/page/aboutpage.h"

#include "./frame/base/framelesswindow.h"
#include "./frame/base/sidebar.h"
#include "qwidget.h"


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
        QWidget* _widget {nullptr};
    };

}

