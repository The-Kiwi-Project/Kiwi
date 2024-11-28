#pragma once

#include <QMainWindow>
#include "./page/welcomepage.h"
#include "./page/workpage.h"
#include "./page/aboutpage.h"
#include "./render/renderwidget.h"

namespace kiwi::hardware {
    class Interposer;
}

namespace kiwi::circuit {
    class BaseDie;
}

namespace kiwi::widget {

    // class Window : public FramelessWindow {
    //     Q_OBJECT

    // public:
    //     Window(QWidget *parent = nullptr);
    //     ~Window();

    // private:
    //     void resizePages(QResizeEvent* event);

    // private:
    //     virtual void showEvent(QShowEvent* event) override;
    //     virtual bool eventFilter(QObject* object, QEvent* event) override;

    // private:
    //     QHBoxLayout* _mainLayout = nullptr;
    //     QWidget* _placeHolderWidget = nullptr;
    //     SideBar* _sideBar = nullptr;

    //     WelcomePage* _welcomePage = nullptr;
    //     WorkPage* _workPage = nullptr;
    //     AboutPage* _aboutPage = nullptr;
    // };

    class Window : public QWidget 
    {
        Q_OBJECT

    public:
        Window(hardware::Interposer* i, circuit::BaseDie* b, QWidget *parent = nullptr);
        ~Window();

    protected:
        virtual void showEvent(QShowEvent* event) override;
        virtual bool eventFilter(QObject* object, QEvent* event) override;

    private:
        RenderWidget* _renderWidget {nullptr};
    };

}

